#include "EffectSlot.h"

EffectSlot::EffectSlot()
{
    
}

void EffectSlot::setSampleRate(double sr)
{
    sampleRate = sr;
}

juce::ValueTree EffectSlot::toValueTree()
{
    juce::ValueTree paramTree = juce::ValueTree("effectParams");

    for(int i = 0; i < parameters.size(); i++)
    {
        paramTree.setProperty(juce::Identifier("parameter_" + std::to_string(i)), parameters[i], nullptr);
        paramTree.setProperty(juce::Identifier("parameterName_" + std::to_string(i)), parameterNames[i], nullptr);
        paramTree.setProperty(juce::Identifier("parameterRangeLow_" + std::to_string(i)), parameterRanges[i][0], nullptr);
        paramTree.setProperty(juce::Identifier("parameterRangeHigh_" + std::to_string(i)), parameterRanges[i][1], nullptr);
        paramTree.setProperty(juce::Identifier("parameterSkew_" + std::to_string(i)), parameterSkews[i], nullptr);
    }

    return paramTree;
}

void EffectSlot::fromValueTree(const juce::ValueTree &paramTree)
{
    for(int i = 0; i < parameters.size(); i++)
    {
        parameters[i] = paramTree.getProperty(juce::Identifier("parameter_" + std::to_string(i)));
        parameterNames[i] = paramTree.getProperty(juce::Identifier("parameterName_" + std::to_string(i)));
        parameterRanges[i][0] = paramTree.getProperty(juce::Identifier("parameterRangeLow_" + std::to_string(i)));
        parameterRanges[i][1] = paramTree.getProperty(juce::Identifier("parameterRangeHigh_" + std::to_string(i)));
        parameterSkews[i] = paramTree.getProperty(juce::Identifier("parameterSkew_" + std::to_string(i)));
    }
}