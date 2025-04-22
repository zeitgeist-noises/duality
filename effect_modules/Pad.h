#pragma once

#include <JuceHeader.h>
#include "../EffectSlot.h"

class Pad : public EffectSlot
{
public:
    enum Parameters
    {
        length
    };

    Pad();
    ~Pad() = default;
    void apply(juce::AudioBuffer<float> &dry, std::vector<float> parameters);
    juce::String getEffectName();
};