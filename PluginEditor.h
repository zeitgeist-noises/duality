#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DragExportButton.h"

//==============================================================================
class DualityAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                          public juce::FileDragAndDropTarget,
                                          public juce::Slider::Listener,
                                          public juce::ComboBox::Listener,
                                          public juce::ChangeListener
{
public:
    explicit DualityAudioProcessorEditor (DualityAudioProcessor&);
    ~DualityAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y);
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox *box) override;
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DualityAudioProcessor& processorRef;

    //gui controls

    juce::TextButton openButton;
    void openButtonClicked();

    juce::TextButton playButton;
    void playButtonClicked();

    juce::TextButton stopButton;
    void stopButtonClicked();

    juce::TextButton transformButton;
    void transformButtonClicked();

    juce::ToggleButton transformOnlyToggle;
    void toggleClicked();

    juce::TextButton saveButton;
    void saveButtonClicked();

    std::vector<juce::Slider> sliders;
    std::vector<juce::Label> sliderLabels;

    juce::ComboBox effectList;
    void effectSelected();

    juce::ComboBox modeList;
    void modeSelected();
    const juce::StringArray modeNames = {"flip f=t", "flip f=fmax-t", "+pi/2", "-pi/2", "+pi"};

    juce::Label sourceLabel;
    juce::Label transformLabel;
    juce::Label outputLabel;
    DragExportButton dragButton;

    //loading and drawing files
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::File sourceFile;
    juce::File transformedFile;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail sourceWaveform;
    juce::AudioThumbnail transformedWaveform;
    void loadFilesIntoEditor();

    //globals
    const juce::Colour bgColour = juce::Colours::black;
    const juce::Colour drawColour = juce::Colours::whitesmoke;
    const juce::Colour highlightColour = juce::Colours::blueviolet;
    const int margin = 20;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DualityAudioProcessorEditor)
};
