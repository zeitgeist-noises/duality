#pragma once

#include <JuceHeader.h>

class EffectSlot
{
public:
    static inline const juce::StringArray effectNames = {"distortion", "noise convolution", "pad", "delay", "sample skew"};

    EffectSlot();
    virtual ~EffectSlot() = default;
    virtual void apply(juce::AudioBuffer<float> &dry, std::vector<float> parameters) = 0;
    static void setSampleRate(double sr);

    std::vector<float> parameterDefaults;
    std::vector<juce::String> parameterNames;
    std::vector<std::vector<float>> parameterRanges;
    std::vector<float> parameterSkews;

    virtual juce::String getEffectName() = 0;

    float skewFunction(float input, int index);
    
protected:
    static inline double sampleRate = 0.0;

};