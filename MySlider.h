#pragma once

#include <JuceHeader.h>

class MySlider : public juce::Slider
{
public:
    MySlider()
    {
        range = {0.0f, 1.0f};
        skew = 1.0f;

        textFromValueFunction = [this](double value){return tfv(value);};
        valueFromTextFunction = [this](juce::String text){return vft(text);};
    }

    std::vector<float> range;
    float skew;

private:
    juce::String tfv(double value)
    {
        return std::to_string(range[0] + (range[1] - range[0])*pow(value, skew));
    }

    double vft(juce::String text)
    {
        double value = std::stod(text.toStdString());
        return pow((value - range[0])/(range[1]-range[0]), 1/skew);
    }
};