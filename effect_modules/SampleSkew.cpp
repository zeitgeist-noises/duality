#include "SampleSkew.h"

SampleSkew::SampleSkew()
{
    parameterNames = {"skew", "dry/wet", "", ""};
    parameters = {0.5f, 1.0f, 1.0f, 1.0f};
    parameterRanges = {{0.0, 1.0}, {0.0, 1.0}, {0.0, 1.0}, {0.0, 1.0}};
    parameterSkews = {1.0, 1.0, 1.0, 1.0};
}

void SampleSkew::apply(juce::AudioBuffer<float> &dry)
{
    int numSamples = dry.getNumSamples();
    int numChannels = dry.getNumChannels();
    AudioBuffer<float> wet (numChannels, numSamples);

    for(int channel = 0; channel < numChannels; channel++)
    {
        const float *dryBuffer = dry.getReadPointer(channel);
        float *wetBuffer = wet.getWritePointer(channel);

        for(int sample = 0; sample < numSamples; sample++)
        {
            double position = static_cast<double>(sample) / numSamples;
            int newSample = static_cast<int>(numSamples*skewFunc(position));

            *(wetBuffer + sample) = *(dryBuffer + newSample);
        }
    }

    //dry wet
    dry.applyGain(1.0f - parameters[SampleSkew::drywet]);
    wet.applyGain(parameters[SampleSkew::drywet]);

    for(int channel = 0; channel < dry.getNumChannels(); channel++)
        dry.addFrom(channel, 0, wet, channel, 0, numSamples);
}

juce::String SampleSkew::getEffectName()
{
    return EffectSlot::effectNames[4];
}

double SampleSkew::skewFunc(double x)
{
    double a = static_cast<double>(parameters[SampleSkew::skew]);
    if(a == 0.0)
        return 0.0;
    
    return pow(x, a/(1-a));
}
