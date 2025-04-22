#include "Distortion.h"

Distortion::Distortion()
{
    parameterNames = {"drive dB", "dry/wet", "", ""};
    parameterDefaults = {0.0f, 1.0f, 0.5f, 0.5f};
    parameterRanges = {{0.0f, 50.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}};
    parameterSkews = {1.0f, 1.0f, 1.0f, 1.0f};
}

void Distortion::apply(juce::AudioBuffer<float> &dry, std::vector<float> parameters)
{
    float drive = skewFunction(parameters[Distortion::drive], Distortion::drive);
    float drywet = parameters[Distortion::drywet];

    juce::AudioBuffer<float> wet(dry);

    float gain = powf(10, drive / 20);
    wet.applyGain(gain);

    for(int channel = 0; channel < dry.getNumChannels(); channel++)
    {
        float *buffer = wet.getWritePointer(channel);
        for(int sample = 0; sample < dry.getNumSamples(); sample++)
        {
            if(*(buffer+sample) >= 1.0f)
                *(buffer + sample) = 1.0f;
            else if(*(buffer+sample) < -1.0f)
                *(buffer + sample) = -1.0f;
        }
    }
    
    //dry wet
    dry.applyGain(1.0f - drywet);
    wet.applyGain(drywet);
    
    for(int channel = 0; channel < dry.getNumChannels(); channel++)
        dry.addFrom(channel, 0, wet, channel, 0, wet.getNumSamples());
}

juce::String Distortion::getEffectName()
{
    return EffectSlot::effectNames[0];
}