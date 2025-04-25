#pragma once

#include <JuceHeader.h>

class MySlider : public juce::Slider
{
public:
    MySlider()
    {
        range = {0.0f, 1.0f};
        skew = 1.0f;
    }

    double getValueFromText(const juce::String &text) override
    {
        double value = std::stod(text.toStdString());
        return pow((value - range[0])/(range[1]-range[0]), skew);
    }
    juce::String getTextFromValue(double value) override
    {
        return std::to_string(range[0] + (range[1] - range[0])*pow(value, 1/skew));
    }

    std::vector<float> range;
    float skew;
};