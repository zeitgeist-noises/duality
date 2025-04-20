#pragma once

#include <JuceHeader.h>
#include <vector>

class ModeVisualizer
{
    //x,y start 30, 275
    //x,y end   290, 365
public:
    void draw(juce::Graphics &g);
    void setMode(juce::String m);

private:
    juce::String mode;
    juce::Point<int> origin;
    juce::Point<int> tTip;
    juce::Point<int> fTip;
    std::vector<juce::String> axes;

    void loadMode(int x0, int y0);
};