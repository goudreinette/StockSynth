/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "stock_data.h"
#include "cfloat"

//==============================================================================
StockSynthAudioProcessor::StockSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}


void StockSynthAudioProcessor::calculateMaxTick(int index) {
    currentStock = index;
    auto v = stock_data[index];
    maxTick = *std::max_element(v.begin(), v.end());
    minTick = *std::min_element(v.begin(), v.end());
}


StockSynthAudioProcessor::~StockSynthAudioProcessor()
{
}


float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {

    if (fabs(inputMin - inputMax) < FLT_EPSILON){
        return outputMin;
    } else {
        float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

        if( clamp ){
            if(outputMax < outputMin){
                if( outVal < outputMax )outVal = outputMax;
                else if( outVal > outputMin )outVal = outputMin;
            }else{
                if( outVal > outputMax )outVal = outputMax;
                else if( outVal < outputMin )outVal = outputMin;
            }
        }
        return outVal;
    }

}




//==============================================================================
const String StockSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StockSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StockSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StockSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StockSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StockSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StockSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StockSynthAudioProcessor::setCurrentProgram (int index)
{
}

const String StockSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void StockSynthAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void StockSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    currentSampleRate = sampleRate;
}

void StockSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StockSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void StockSynthAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    MidiMessage msg;
    int time;
    for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(msg, time);)
    {
        if (msg.isNoteOn()) {
            note = msg.getNoteNumber();
            is_released = false;
        }
        if (msg.isNoteOff()) {
            is_released = true;
        }
    }

        vector<float> current_stock_data = stock_data[currentStock];
        int size = stock_data[currentStock].size();
        double freq = MidiMessage::getMidiNoteInHertz(note);

        double per_sample = 1.0 / currentSampleRate;
        double attack_per_sample = per_sample * (1.0 / attack_time);
        double release_per_sample = per_sample * (1.0 / release_time);


    for (auto sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            auto cyclesPerSample = frequency / currentSampleRate;
            auto angleDelta = cyclesPerSample * 2.0 + MathConstants<double>::pi;
            auto currentAngle = t * angleDelta;
            t += cyclesPerSample;

            if (!is_released && alpha < 1.0) {
                alpha += attack_per_sample;
            }

            if (is_released && alpha > 0.0) {
                alpha -= release_per_sample;
            }

            if (alpha <= 0.0) {
                note = -1;
            }


            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                auto* channelData = buffer.getWritePointer (channel);

                if (note != -1) {
                    channelData[sample] = ofMap(current_stock_data[int (freq * t) % size], minTick, maxTick, -1, 1, false) * alpha;
                }

                        //current_stock_data[int (frequency * t) % size] / maxTick;

                        //std::sin(currentAngle);
            }

//        if (note != -1) {
//             *channelData =

//            *channelData = stock_data[currentStock][(int)((int)(frequency * time) % stock_data[currentStock].size())] / maxTick;
//        }
        // ..do something to the data...
    }
}

//==============================================================================
bool StockSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StockSynthAudioProcessor::createEditor()
{
    return new StockSynthAudioProcessorEditor (*this);
}

//==============================================================================
void StockSynthAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void StockSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StockSynthAudioProcessor();
}
