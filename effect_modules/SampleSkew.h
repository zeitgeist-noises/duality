#pragma once

#include <JuceHeader.h>
#include "../EffectSlot.h"

class SampleSkew : public EffectSlot
{
public:
    enum Parameters
    {
        skew,
        drywet
    };

    SampleSkew();
    ~SampleSkew() = default;
    void apply(juce::AudioBuffer<float> &dry);
    juce::String getEffectName();

private:
    double skewFunc(double x);
};