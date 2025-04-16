#pragma once

#include <JuceHeader.h>

class DragExportButton : public juce::TextButton, public juce::DragAndDropContainer
{
public:
    void mouseDrag(const MouseEvent &event) override
    {
        ignoreUnused(event);
        juce::File toExport = juce::File::createTempFile(".wav");
        transformed.copyFileTo(toExport);
        performExternalDragDropOfFiles(toExport.getFullPathName(), true);
    }

private:
    juce::File transformed = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                                    .getChildFile("duality").getChildFile("transformed.wav");
};