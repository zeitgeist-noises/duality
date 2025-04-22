#pragma once

#include <JuceHeader.h>
#include "../EffectSlot.h"

class Distortion : public EffectSlot
{
public:
    enum Parameters
    {
        drive,
        drywet
    };

    Distortion();
    ~Distortion() = default;
    void apply(juce::AudioBuffer<float> &dry, std::vector<float> parameters);
    juce::String getEffectName();
};
