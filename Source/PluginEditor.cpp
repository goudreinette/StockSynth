/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "vector"
#include "stock_data.h"
#include <algorithm>
#include <iostream>


using namespace std;

//==============================================================================
StockSynthAudioProcessorEditor::StockSynthAudioProcessorEditor (StockSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    startTimerHz(60);

    addAndMakeVisible (stockComboBox);
    stockComboBox.addItem ("AMD", 1);
    stockComboBox.addItem ("TSLA", 2);
    stockComboBox.addItem ("GOOGL", 3);
    stockComboBox.addItem ("NYT", 4);
    stockComboBox.addItem ("MCD", 5);
    stockComboBox.setSelectedItemIndex(0, true);
    stockComboBox.onChange = [this] { processor.calculateMaxTick(stockComboBox.getSelectedItemIndex()); };
    processor.calculateMaxTick(stockComboBox.getSelectedItemIndex());

//    addAndMakeVisible (frequencySlider);
    frequencySlider.setRange (50, 5000.0);          // [1]
    frequencySlider.setTextValueSuffix (" Hz");     // [2]
    frequencySlider.onValueChange = [this] { processor.frequency = frequencySlider.getValue(); };

    // Attack
    attackSlider.setRange(0.01, 5.0);
    attackSlider.setTextValueSuffix ("s");     // [2]
    attackSlider.setNumDecimalPlacesToDisplay(2);
    attackSlider.setValue(processor.attack_time);
    attackSlider.onValueChange = [this] { processor.attack_time = attackSlider.getValue(); };
    addAndMakeVisible(attackSlider);

    // Release
    releaseSlider.setRange(0.01, 5.0);
    releaseSlider.setTextValueSuffix ("s");     // [2]
    releaseSlider.setNumDecimalPlacesToDisplay(2);
    releaseSlider.setValue(processor.release_time);
    releaseSlider.onValueChange = [this] { processor.release_time = releaseSlider.getValue(); };
    addAndMakeVisible(releaseSlider);
}




StockSynthAudioProcessorEditor::~StockSynthAudioProcessorEditor()
{
}

//==============================================================================
void StockSynthAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    int index = stockComboBox.getSelectedItemIndex();

    g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText (std::to_string(index), getLocalBounds(), Justification::centred, 1);
    g.drawFittedText (std::to_string(processor.maxTick), getLocalBounds(), Justification::bottom, 1);

    std::vector<float> ticks = stock_data[index];

    float stepSize = getWidth() / (float) ticks.size();

    for (int i = 0; i < ticks.size(); ++i) {
        float tick = ticks[i] / processor.maxTick;

        g.fillRect(i*stepSize, tick * 200, stepSize, getHeight() - tick*200);
    }
}

void StockSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    stockComboBox.setBounds (20, 20, getWidth() -40, 40);
//    frequencySlider.setBounds (20, 20, 400, 20);
    attackSlider.setBounds(10, getHeight()-30, getWidth()/2-10, 20);
    releaseSlider.setBounds(getWidth()/2, getHeight()-30, getWidth() / 2-10, 20);
}
