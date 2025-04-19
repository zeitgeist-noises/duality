#pragma once

#include <JuceHeader.h>
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