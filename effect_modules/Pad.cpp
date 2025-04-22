#include "Pad.h"

Pad::Pad()
{
    parameterNames = {"length", "", "", ""};
    parameterDefaults = {1.0f, 1.0f, 1.0f, 1.0f};
    parameterRanges = {{0.0f, 10.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}};
    parameterSkews = {1.0f, 1.0f, 1.0f, 1.0f};
}

void Pad::apply(juce::AudioBuffer<float> &dry, std::vector<float> parameters)
{
    float length = skewFunction(parameters[Pad::length], Pad::length);

    int new_length = static_cast<int>(dry.getNumSamples()*length);
    dry.setSize(dry.getNumChannels(), new_length, true, true);
}

juce::String Pad::getEffectName()
{
    return EffectSlot::effectNames[2];
}
