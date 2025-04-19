#pragma once

#include <JuceHeader.h>

class EffectSlot
{
public:
    static inline const juce::StringArray effectNames = {"distortion", "noise convolution", "pad", "delay", "sample skew"};

    EffectSlot();
    virtual ~EffectSlot() = default;
    virtual void apply(juce::AudioBuffer<float> &dry) = 0;
    static void setSampleRate(double sr);

    std::vector<float> parameters;
    std::vector<juce::String> parameterNames;
    std::vector<std::vector<double>> parameterRanges;
    std::vector<double> parameterSkews;

    //serialization
    juce::ValueTree toValueTree();
    void fromValueTree(const juce::ValueTree& paramTree);
    virtual juce::String getEffectName() = 0;
    
protected:
    static inline double sampleRate = 0.0;

};