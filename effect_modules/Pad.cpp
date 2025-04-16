#include "Pad.h"

Pad::Pad()
{
    parameterNames = {"length", "", "", ""};
    parameters = {1.0f, 1.0f, 1.0f, 1.0f};
    parameterRanges = {{0.0, 10.0}, {0.0, 1.0}, {0.0, 1.0}, {0.0, 1.0}};
    parameterSkews = {1.0, 1.0, 1.0, 1.0};
}

void Pad::apply(juce::AudioBuffer<float> &dry)
{
    int new_length = static_cast<int>(dry.getNumSamples()*parameters[Pad::length]);
    dry.setSize(dry.getNumChannels(), new_length, true, true);
}

juce::String Pad::getEffectName()
{
    return EffectSlot::effectNames[2];
}
