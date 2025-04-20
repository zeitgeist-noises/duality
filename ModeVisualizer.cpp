#include "ModeVisualizer.h"

void ModeVisualizer::draw(juce::Graphics &g)
{
    int x0 = 30;
    int y0 = 275;

    //left
    g.drawArrow(juce::Line<float>(x0+30.0f, y0+70.0f, x0+30.0f, y0+20.0f), 3.0f, 9.0f, 9.0f);
    g.drawArrow(juce::Line<float>(x0+30.0f, y0+70.0f, x0+80.0f, y0+70.0f), 3.0f, 9.0f, 9.0f);
    g.drawText("f", x0+15, y0+40, 10, 10, juce::Justification::centred);
    g.drawText("t", x0+50, y0+75, 10, 10, juce::Justification::centred);

    //middle
    g.drawArrow(juce::Line<float>(x0+100.0f, y0+45.0f, x0+150.0f, y0+45.0f), 7.0f, 21.0f, 21.0f);
    
    //right
    x0 += 180;
    y0 += 20;

    loadMode(x0, y0);
    
    g.drawArrow(juce::Line<int>(origin, tTip).toFloat(), 3.0f, 9.0f, 9.0f);
    g.drawArrow(juce::Line<int>(origin, fTip).toFloat(), 3.0f, 9.0f, 9.0f);

    std::vector<std::vector<int>> positions = {{20, -15}, {55, 20}, {20, 55}, {-15, 20}};
    for(int i = 0; i < positions.size(); i++)
    {
        g.drawText(axes[i], x0+positions[i][0], y0+positions[i][1], 10, 10, juce::Justification::centred);
    }
}

void ModeVisualizer::setMode(juce::String m)
{
    mode = m;
}

void ModeVisualizer::loadMode(int x0, int y0)
{
    int length = 50;

    if(mode == "flip f=t")
    {
        origin = juce::Point<int>(x0, y0+length);
        tTip = juce::Point<int>(x0, y0);
        fTip = juce::Point<int>(x0+length, y0+length);
        axes = {"", "", "f", "t"};
    }
    else if(mode == "flip f=fmax-t")
    {
        origin = juce::Point<int>(x0+length, y0);
        tTip = juce::Point<int>(x0+length, y0+length);
        fTip = juce::Point<int>(x0, y0);
        axes = {"f", "t", "", ""};
    }
    else if(mode == "+pi/2")
    {
        origin = juce::Point<int>(x0+length, y0+length);
        tTip = juce::Point<int>(x0+length, y0);
        fTip = juce::Point<int>(x0, y0+length);
        axes = {"", "t", "f", ""};
    }
    else if(mode == "-pi/2")
    {
        origin = juce::Point<int>(x0, y0);
        tTip = juce::Point<int>(x0, y0+length);
        fTip = juce::Point<int>(x0+length, y0);
        axes = {"f", "", "", "t"};
    }
    else if(mode == "+pi")
    {
        origin = juce::Point<int>(x0+length, y0);
        tTip = juce::Point<int>(x0, y0);
        fTip = juce::Point<int>(x0+length, y0+length);
        axes = {"t", "f", "", ""};
    }
}
