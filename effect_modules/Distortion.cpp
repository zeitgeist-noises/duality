#include "Distortion.h"

Distortion::Distortion()
{
    parameterNames = {"drive dB", "dry/wet", "", ""};
    parameters = {0.0f, 1.0f, 0.5f, 0.5f};
    parameterRanges = {{0.0, 50.0}, {0.0, 1.0}, {0.0, 1.0}, {0.0, 1.0}};
    parameterSkews = {1.0, 1.0, 1.0, 1.0};
}

void Distortion::apply(juce::AudioBuffer<float> &dry)
{
    juce::AudioBuffer<float> wet(dry);

    float gain = powf(10, parameters[Distortion::drive] / 20);
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
    dry.applyGain((1.0f - parameters[Distortion::drywet]));
    wet.applyGain(parameters[Distortion::drywet]);
    
    for(int channel = 0; channel < dry.getNumChannels(); channel++)
        dry.addFrom(channel, 0, wet, channel, 0, wet.getNumSamples());
}

juce::String Distortion::getEffectName()
{
    return EffectSlot::effectNames[0];
}