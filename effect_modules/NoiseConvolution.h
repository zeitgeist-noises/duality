#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "../EffectSlot.h"
#include <random>
#include <cmath>

class NoiseConvolution : public EffectSlot
{
public:
    enum Parameters
    {
        decay,
        order,
        width,
        drywet
    };

    NoiseConvolution();
    ~NoiseConvolution() = default;
    void apply(juce::AudioBuffer<float> &dry);
    juce::String getEffectName();

private:
    juce::AudioBuffer<float> createIR(int numSamples);
};