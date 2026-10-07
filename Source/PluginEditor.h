#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <array>

class PaperDrumsAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    explicit PaperDrumsAudioProcessorEditor(PaperDrumsAudioProcessor&);
    ~PaperDrumsAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;

private:
    void timerCallback() override;
    void drawKick(juce::Graphics&, juce::Rectangle<float>, float glow);
    void drawSnare(juce::Graphics&, juce::Rectangle<float>, float glow);
    void drawHat(juce::Graphics&, juce::Rectangle<float>, float glow);
    void drawTear(juce::Graphics&, juce::Rectangle<float>, float glow);
    void drawCrumple(juce::Graphics&, juce::Rectangle<float>, float glow);
    void drawLabel(juce::Graphics&, const juce::String&, juce::Point<float>, float size,
                   juce::Colour colour, juce::Justification justification = juce::Justification::centred);
    void drawIndicator(juce::Graphics&, juce::Point<float>, float glow, juce::Colour);
    int padAt(juce::Point<float>) const;

    PaperDrumsAudioProcessor& processor;
    std::array<juce::Rectangle<float>, PaperDrumsAudioProcessor::padCount> padAreas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PaperDrumsAudioProcessorEditor)
};
