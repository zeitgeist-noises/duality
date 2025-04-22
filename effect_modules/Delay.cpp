#include "Delay.h"

Delay::Delay()
{
    parameterNames = {"delays", "feedback", "dry/wet", ""};
    parameterDefaults = {1.0f, 0.5f, 1.0f, 1.0f};
    parameterRanges = {{0.0f, 100.0f}, {0.0f, 2.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}};
    parameterSkews = {1.0f, 1.0f, 1.0f, 1.0f};
}

void Delay::apply(juce::AudioBuffer<float> &dry, std::vector<float> parameters)
{
    float delays = skewFunction(parameters[Delay::delays], Delay::delays);
    float feedback = skewFunction(parameters[Delay::feedback], Delay::feedback);
    float drywet = parameters[Delay::drywet];

    int numSamples = dry.getNumSamples();
    int numChannels = dry.getNumChannels();
    juce::AudioBuffer<float> wet (numChannels, numSamples);
    wet.clear();

    int delayBufferSize = static_cast<int>(numSamples/(1+delays));
    
    int wholeDelays = static_cast<int>(std::ceilf(delays));
    for(int i = 0; i < wholeDelays; i++)
    {
        int sample = (i+1)*delayBufferSize;
        for(int channel = 0; channel < numChannels; channel++)
        {
            wet.addFrom(channel, sample, dry, channel, 0, numSamples - sample, powf(feedback, static_cast<float>(i)));
        }
    }

    //dry wet
    wet.applyGain(1.0f / wet.getMagnitude(0, numSamples));
    dry.applyGain(1.0f - drywet);
    wet.applyGain(drywet / wet.getMagnitude(0, numSamples));


    for(int channel = 0; channel < dry.getNumChannels(); channel++)
        dry.addFrom(channel, 0, wet, channel, 0, numSamples);
}

juce::String Delay::getEffectName()
{
    return EffectSlot::effectNames[3];
}
