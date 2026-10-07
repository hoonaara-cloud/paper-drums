#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>

class PaperDrumsAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int padCount = 5;

    enum Pad : int
    {
        Kick = 0,
        Snare,
        Hat,
        Tear,
        Crumple
    };

    PaperDrumsAudioProcessor();
    ~PaperDrumsAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Paper Drums"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Called by the editor. The audio thread consumes these requests at the
    // beginning of the next block, so the editor never touches voice memory.
    void firePad(int pad, float velocity = 1.0f);
    float getPadActivity(int pad) const;
    void decayPadActivity(float amount = 0.82f);
    int getMidiNoteForPad(int pad) const;
    const juce::String& getPadName(int pad) const;

    juce::AudioProcessorValueTreeState parameters;

private:
    struct Sample
    {
        juce::AudioBuffer<float> buffer;
        double sourceRate = 44100.0;
        bool valid = false;
    };

    struct Voice
    {
        int pad = -1;
        double sourcePosition = 0.0;
        double increment = 1.0;
        int samplesPlayed = 0;
        int maxHostSamples = 0;
        float gain = 0.0f;
        unsigned long long age = 0;
        int releaseSamplesRemaining = 0;
        bool releasing = false;
        bool active = false;
    };

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void loadSamples();
    void triggerPadOnAudioThread(int pad, float velocity);
    void renderVoices(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

    double currentSampleRate = 44100.0;
    std::array<Sample, padCount> samples;
    std::array<Voice, 32> voices;
    std::array<std::atomic<float>, padCount> pendingVelocities;
    std::array<std::atomic<float>, padCount> padActivity;
    unsigned long long voiceAge = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PaperDrumsAudioProcessor)
};
