#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DragExportButton.h"
#include "ModeVisualizer.h"
#include "MySlider.h"

#define APVTS AudioProcessorValueTreeState

//==============================================================================
class DualityAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                          public juce::FileDragAndDropTarget,
                                          private juce::ChangeListener
{
public:
    explicit DualityAudioProcessorEditor (DualityAudioProcessor&);
    ~DualityAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y);

    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    

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
    std::optional<juce::APVTS::ButtonAttachment> transformOnlyAttachment;
    void toggleClicked();

    juce::TextButton saveButton;
    void saveButtonClicked();

    std::vector<MySlider> sliders;
    std::vector<juce::Label> sliderLabels;
    std::optional<juce::APVTS::SliderAttachment> sliderAttachment0;
    std::optional<juce::APVTS::SliderAttachment> sliderAttachment1;
    std::optional<juce::APVTS::SliderAttachment> sliderAttachment2;
    std::optional<juce::APVTS::SliderAttachment> sliderAttachment3;
    void handleSliders();

    juce::ComboBox effectList;
    std::optional<juce::APVTS::ComboBoxAttachment> effectAttachment;
    void effectSelected();
    bool isFirstLoad = true;

    juce::ComboBox modeList;
    std::optional<juce::APVTS::ComboBoxAttachment> modeAttachment;
    void modeSelected();
    ModeVisualizer modeUI;

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
    const juce::Colour highlightColour = juce::Colours::grey;
    const int margin = 20;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DualityAudioProcessorEditor)
};
