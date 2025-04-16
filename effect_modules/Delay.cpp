#include "Delay.h"

Delay::Delay()
{
    parameterNames = {"delays", "feedback", "dry/wet", ""};
    parameters = {1.0f, 0.5f, 1.0f, 1.0f};
    parameterRanges = {{0.0, 100.0}, {0.0, 2.0}, {0.0, 1.0}, {0.0, 1.0}};
    parameterSkews = {1.0, 1.0, 1.0, 1.0};
}

void Delay::apply(juce::AudioBuffer<float> &dry)
{
    int numSamples = dry.getNumSamples();
    int numChannels = dry.getNumChannels();
    juce::AudioBuffer<float> wet (numChannels, numSamples);
    wet.clear();

    int delayBufferSize = static_cast<int>(numSamples/(1+parameters[Delay::delays]));
    
    int wholeDelays = static_cast<int>(std::ceilf(parameters[Delay::delays]));
    for(int i = 0; i < wholeDelays; i++)
    {
        int sample = (i+1)*delayBufferSize;
        for(int channel = 0; channel < numChannels; channel++)
        {
            wet.addFrom(channel, sample, dry, channel, 0, numSamples - sample, powf(parameters[Delay::feedback], static_cast<float>(i)));
        }
    }

    //dry wet
    wet.applyGain(1.0f / wet.getMagnitude(0, numSamples));
    dry.applyGain(1.0f - parameters[Delay::drywet]);
    wet.applyGain(parameters[Delay::drywet] / wet.getMagnitude(0, numSamples));


    for(int channel = 0; channel < dry.getNumChannels(); channel++)
        dry.addFrom(channel, 0, wet, channel, 0, numSamples);
}

juce::String Delay::getEffectName()
{
    return EffectSlot::effectNames[3];
}
