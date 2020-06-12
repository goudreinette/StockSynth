/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class StockSynthAudioProcessorEditor  : public AudioProcessorEditor, private Timer
{
public:
    StockSynthAudioProcessorEditor (StockSynthAudioProcessor&);
    ~StockSynthAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void timerCallback() final {
        repaint();
    }

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StockSynthAudioProcessor& processor;
    Slider frequencySlider;
    Slider attackSlider;
    Slider releaseSlider;
    ComboBox stockComboBox;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StockSynthAudioProcessorEditor)
};
