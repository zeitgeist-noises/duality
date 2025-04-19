#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "effect_modules/Distortion.h"
#include "effect_modules/NoiseConvolution.h"
#include "effect_modules/Pad.h"
#include "effect_modules/Delay.h"
#include "effect_modules/SampleSkew.h"

//==============================================================================
class DualityAudioProcessor final : public juce::AudioProcessor, public juce::ChangeListener
{
public:

    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    //==============================================================================
    DualityAudioProcessor();
    ~DualityAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    //MY STUFF
    void changeState(TransportState newState);
    void loadFile(juce::File &filename);
    void process(const juce::File &inputFile, juce::File &transformedFile);

    EffectSlot *effect;
    void setEffect(juce::String effectType);

    juce::String transformMode;
    bool transformOnly = false;

    void addTreeChild(juce::ValueTree &parent,
                        const juce::Identifier &childType,
                        const juce::Identifier &propertyName,
                        const juce::var &propertyValue);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DualityAudioProcessor)
    //MY STUFF
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    void ifft(juce::AudioBuffer<float> &data);
    void transform(juce::AudioBuffer<float> &data);
    void itransform(juce::AudioBuffer<float> &data);


    void applyEffects(juce::AudioBuffer<float> &data);
};