#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    struct PadSpec
    {
        const void* data;
        int dataSize;
        int midiNote;
        float pitch;
        float gain;
        float maxSeconds;
    };
}

PaperDrumsAudioProcessor::PaperDrumsAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "STATE", createParameterLayout())
{
    for (auto& value : pendingVelocities)
        value.store(0.0f);

    for (auto& value : padActivity)
        value.store(0.0f);
}

juce::AudioProcessorValueTreeState::ParameterLayout PaperDrumsAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "master", "Master", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.9f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "paper", "Paper", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.8f));

    return { params.begin(), params.end() };
}

void PaperDrumsAudioProcessor::decayPadActivity(float amount)
{
    amount = juce::jlimit(0.0f, 1.0f, amount);

    for (int i = 0; i < padCount; ++i)
    {
        auto& value = padActivity[static_cast<size_t>(i)];
        value.store(value.load() * amount);
    }
}
void PaperDrumsAudioProcessor::releaseResources()
{
    for (auto& voice : voices)
        voice.active = false;
}

bool PaperDrumsAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono()
        || output == juce::AudioChannelSet::stereo();
}

void PaperDrumsAudioProcessor::loadSamples()
{
    using namespace PaperDrumsAssets;

    const std::array<PadSpec, padCount> specs = {{
        { paper_handling_wav, paper_handling_wavSize, 36, 0.42f, 0.95f, 0.55f },
        { paper_handling_wav, paper_handling_wavSize, 38, 0.86f, 0.72f, 0.34f },
        { pencil_on_paper_wav, pencil_on_paper_wavSize, 42, 1.70f, 0.32f, 0.18f },
        { paper_tear_wav, paper_tear_wavSize, 46, 1.05f, 0.80f, 0.78f },
        { paper_crumple_wav, paper_crumple_wavSize, 49, 1.00f, 0.78f, 0.52f }
    }};

    juce::WavAudioFormat wav;

    for (int i = 0; i < padCount; ++i)
    {
        samples[static_cast<size_t>(i)] = {};

        auto input = std::make_unique<juce::MemoryInputStream>(
            specs[static_cast<size_t>(i)].data,
            static_cast<size_t>(specs[static_cast<size_t>(i)].dataSize),
            false);

        std::unique_ptr<juce::AudioFormatReader> reader(wav.createReaderFor(input.release(), true));
        if (reader == nullptr || reader->lengthInSamples <= 0)
            continue;

        auto& destination = samples[static_cast<size_t>(i)];
        const auto length = static_cast<int>(juce::jmin<int64>(reader->lengthInSamples, 4800000));
        destination.buffer.setSize(1, length);
        reader->read(&destination.buffer, 0, length, 0, true, false);
        destination.sourceRate = reader->sampleRate;
        destination.valid = true;
    }
}

void PaperDrumsAudioProcessor::firePad(int pad, float velocity)
{
    if (pad < 0 || pad >= padCount)
        return;

    velocity = juce::jlimit(0.0f, 1.0f, velocity);
    padActivity[static_cast<size_t>(pad)].store(1.0f);

    auto& pending = pendingVelocities[static_cast<size_t>(pad)];
    auto oldValue = pending.load();
    while (oldValue < velocity && !pending.compare_exchange_weak(oldValue, velocity)) {}
}

void PaperDrumsAudioProcessor::triggerPadOnAudioThread(int pad, float velocity)
{
    if (pad < 0 || pad >= padCount || ! samples[static_cast<size_t>(pad)].valid)
        return;

    const auto& sample = samples[static_cast<size_t>(pad)];
    const std::array<float, padCount> pitches = {{ 0.42f, 0.86f, 1.70f, 1.05f, 1.00f }};
    const std::array<float, padCount> gains = {{ 0.95f, 0.72f, 0.32f, 0.80f, 0.78f }};
    const std::array<float, padCount> durations = {{ 0.55f, 0.34f, 0.18f, 0.78f, 0.52f }};

    Voice* selected = nullptr;
    for (auto& voice : voices)
    {
        if (!voice.active)
        {
            selected = &voice;
            break;
        }

        if (selected == nullptr || voice.age < selected->age)
            selected = &voice;
    }

    if (selected == nullptr)
        return;

    selected->pad = pad;
    selected->sourcePosition = 0.0;
    selected->increment = (sample.sourceRate / currentSampleRate) * pitches[static_cast<size_t>(pad)];
    selected->samplesPlayed = 0;
    selected->maxHostSamples = juce::jmax(1, static_cast<int>(durations[static_cast<size_t>(pad)] * currentSampleRate));
    selected->gain = gains[static_cast<size_t>(pad)] * velocity;
    selected->age = ++voiceAge;
    selected->releaseSamplesRemaining = 0;
    selected->releasing = false;
    selected->active = true;

    padActivity[static_cast<size_t>(pad)].store(1.0f);
}

void PaperDrumsAudioProcessor::stopPadOnAudioThread(int pad)
{
    if (pad < 0 || pad >= padCount)
        return;

    // A short release avoids a click while still following the MIDI note length.
    const auto releaseLength = juce::jmax(1, static_cast<int>(0.008 * currentSampleRate));

    for (auto& voice : voices)
    {
        if (voice.active && voice.pad == pad)
        {
            voice.releasing = true;
            voice.releaseSamplesRemaining = releaseLength;
        }
    }
}

void PaperDrumsAudioProcessor::renderVoices(juce::AudioBuffer<float>& buffer,
                                            int startSample,
                                            int numSamples)
{
    if (numSamples <= 0)
        return;

    const auto channelCount = buffer.getNumChannels();
    if (channelCount == 0)
        return;

    for (auto& voice : voices)
    {
        if (!voice.active || voice.pad < 0)
            continue;

        const auto& sample = samples[static_cast<size_t>(voice.pad)];
        const auto* source = sample.buffer.getReadPointer(0);
        const auto sourceLength = sample.buffer.getNumSamples();
        const auto releaseLength = juce::jmax(1, static_cast<int>(0.008 * currentSampleRate));

        for (int offset = 0; offset < numSamples; ++offset)
        {
            if (!voice.active || voice.samplesPlayed >= voice.maxHostSamples
                || voice.sourcePosition >= static_cast<double>(sourceLength - 1))
            {
                voice.active = false;
                break;
            }

            const auto index = static_cast<int>(voice.sourcePosition);
            const auto fraction = static_cast<float>(voice.sourcePosition - static_cast<double>(index));
            const auto a = source[index];
            const auto b = source[index + 1];
            auto value = a + (b - a) * fraction;

            const auto remaining = voice.maxHostSamples - voice.samplesPlayed;
            const auto fadeSamples = juce::jmin(256, voice.maxHostSamples / 4);
            if (fadeSamples > 0 && remaining < fadeSamples)
                value *= static_cast<float>(remaining) / static_cast<float>(fadeSamples);

            if (voice.releasing)
            {
                value *= static_cast<float>(voice.releaseSamplesRemaining)
                       / static_cast<float>(releaseLength);

                if (--voice.releaseSamplesRemaining <= 0)
                    voice.active = false;
            }

            value *= voice.gain;

            for (int channel = 0; channel < channelCount; ++channel)
                buffer.addSample(channel, startSample + offset, value);

            voice.sourcePosition += voice.increment;
            ++voice.samplesPlayed;
        }
    }
}

void PaperDrumsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Consume clicks from the paper UI at the start of this audio block.
    for (int pad = 0; pad < padCount; ++pad)
    {
        const auto velocity = pendingVelocities[static_cast<size_t>(pad)].exchange(0.0f);
        if (velocity > 0.0f)
            triggerPadOnAudioThread(pad, velocity);
    }

    int cursor = 0;
    for (const auto metadata : midiMessages)
    {
        const auto eventPosition = juce::jlimit(0, buffer.getNumSamples(), metadata.samplePosition);
        renderVoices(buffer, cursor, eventPosition - cursor);
        cursor = eventPosition;

        const auto message = metadata.getMessage();
        const auto note = message.getNoteNumber();
        const std::array<int, padCount> notes = {{ 36, 38, 42, 46, 49 }};

        for (int pad = 0; pad < padCount; ++pad)
        {
            if (note != notes[static_cast<size_t>(pad)])
                continue;

            if (message.isNoteOn())
                triggerPadOnAudioThread(pad, message.getFloatVelocity());
            else if (message.isNoteOff())
                stopPadOnAudioThread(pad);

            break;
        }
    }

    renderVoices(buffer, cursor, buffer.getNumSamples() - cursor);

    const auto master = parameters.getRawParameterValue("master")->load();
    buffer.applyGain(master);
}

float PaperDrumsAudioProcessor::getPadActivity(int pad) const
{
    if (pad < 0 || pad >= padCount)
        return 0.0f;

    return juce::jlimit(0.0f, 1.0f, padActivity[static_cast<size_t>(pad)].load());
}

void PaperDrumsAudioProcessor::decayPadActivity(float amount)
{
    amount = juce::jlimit(0.0f, 1.0f, amount);
    for (auto& value : padActivity)
        value.store(value.load() * amount);
}

int PaperDrumsAudioProcessor::getMidiNoteForPad(int pad) const
{
    static constexpr int notes[padCount] = { 36, 38, 42, 46, 49 };
    return (pad >= 0 && pad < padCount) ? notes[pad] : -1;
}

const juce::String& PaperDrumsAudioProcessor::getPadName(int pad) const
{
    static const std::array<juce::String, padCount> names = {{
        "KICK", "SNARE", "HAT", "TEAR", "CRUMPLE"
    }};

    static const juce::String unknown = "";
    return (pad >= 0 && pad < padCount) ? names[static_cast<size_t>(pad)] : unknown;
}

void PaperDrumsAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    const auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PaperDrumsAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* PaperDrumsAudioProcessor::createEditor()
{
    return new PaperDrumsAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PaperDrumsAudioProcessor();
}
