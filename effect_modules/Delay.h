#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "../EffectSlot.h"

class Delay : public EffectSlot
{
public:
    enum Parameters
    {
        delays,
        feedback,
        drywet
    };

    Delay();
    ~Delay() = default;
    void apply(juce::AudioBuffer<float> &dry);
    juce::String getEffectName();
};