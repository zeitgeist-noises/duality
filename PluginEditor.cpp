#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DualityAudioProcessorEditor::DualityAudioProcessorEditor (DualityAudioProcessor& p)
    :   AudioProcessorEditor (&p), 
        processorRef(p),
        thumbnailCache(4),
        sourceWaveform(512, formatManager, thumbnailCache),
        transformedWaveform(512, formatManager, thumbnailCache),
        sliders(4),
        sliderLabels(4)
{
    if(!MessageManager::getInstance()->isThisTheMessageThread())
        return;

    formatManager.registerBasicFormats();
    setSize(600, 615);
    
    //gui controls
    addAndMakeVisible(openButton);
    openButton.setColour(openButton.buttonColourId, highlightColour);
    openButton.setColour(openButton.textColourOffId, bgColour);
    openButton.setColour(openButton.textColourOnId, bgColour);
    openButton.setButtonText("open audio file");
    openButton.onClick = [this]{openButtonClicked();};

    addAndMakeVisible(playButton);
    playButton.setColour(playButton.buttonColourId, highlightColour);
    playButton.setColour(playButton.textColourOffId, bgColour);
    playButton.setColour(playButton.textColourOnId, bgColour);
    playButton.setButtonText("play");
    playButton.onClick = [this]{playButtonClicked();};

    addAndMakeVisible(stopButton);
    stopButton.setColour(stopButton.buttonColourId, highlightColour);
    stopButton.setColour(stopButton.textColourOffId, bgColour);
    stopButton.setColour(stopButton.textColourOnId, bgColour);
    stopButton.setButtonText("stop");
    stopButton.onClick = [this]{stopButtonClicked();};
    stopButton.setEnabled(false);

    addAndMakeVisible(transformButton);
    transformButton.setColour(transformButton.buttonColourId, highlightColour);
    transformButton.setColour(transformButton.textColourOffId, bgColour);
    transformButton.setColour(transformButton.textColourOnId, bgColour);
    transformButton.setButtonText("transform");
    transformButton.onClick = [this]{transformButtonClicked();};

    addAndMakeVisible(transformOnlyToggle);
    transformOnlyToggle.setColour(transformOnlyToggle.textColourId, highlightColour);
    transformOnlyToggle.setColour(transformOnlyToggle.tickColourId, highlightColour);
    transformOnlyToggle.setColour(transformOnlyToggle.tickDisabledColourId, highlightColour);
    transformOnlyToggle.setButtonText("tranform\nonly");
    transformOnlyToggle.onClick = [this]{toggleClicked();};

    addAndMakeVisible(saveButton);
    saveButton.setColour(saveButton.buttonColourId, highlightColour);
    saveButton.setColour(saveButton.textColourOffId, bgColour);
    saveButton.setColour(saveButton.textColourOnId, bgColour);
    saveButton.setButtonText("save");
    saveButton.onClick = [this]{saveButtonClicked();};

    addAndMakeVisible(modeList);
    modeList.addItemList(processorRef.modeNames, 1);
    modeList.setColour(juce::ComboBox::ColourIds::backgroundColourId, highlightColour);
    modeList.setColour(juce::ComboBox::ColourIds::textColourId, bgColour);
    modeList.setColour(juce::ComboBox::ColourIds::arrowColourId, bgColour);
    modeList.onChange = [this]{modeSelected();};

    modeUI.setMode(processorRef.transformMode);

    //region labels
    juce::Font labelFont(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 20, Font::bold));

    addAndMakeVisible(sourceLabel);
    sourceLabel.setText("source", juce::dontSendNotification);
    sourceLabel.setJustificationType(juce::Justification::left);
    sourceLabel.setFont(labelFont);
    sourceLabel.setColour(juce::Label::textColourId, drawColour);

    addAndMakeVisible(transformLabel);
    transformLabel.setText("transform", juce::dontSendNotification);
    transformLabel.setJustificationType(juce::Justification::left);
    transformLabel.setFont(labelFont);
    transformLabel.setColour(juce::Label::textColourId, drawColour);

    addAndMakeVisible(outputLabel);
    outputLabel.setText("output", juce::dontSendNotification);
    outputLabel.setJustificationType(juce::Justification::left);
    outputLabel.setFont(labelFont);
    outputLabel.setColour(juce::Label::textColourId, drawColour);

    addAndMakeVisible(dragButton);
    dragButton.setButtonText("or drag from here");
    dragButton.setColour(dragButton.buttonColourId, highlightColour);
    dragButton.setColour(dragButton.textColourOffId, bgColour);
    dragButton.setColour(dragButton.textColourOnId, bgColour);

    addAndMakeVisible(effectList);
    effectList.addItemList(EffectSlot::effectNames, 1);
    effectList.setColour(juce::ComboBox::ColourIds::backgroundColourId, highlightColour);
    effectList.setColour(juce::ComboBox::ColourIds::textColourId, bgColour);
    effectList.setColour(juce::ComboBox::ColourIds::arrowColourId, bgColour);
    effectList.onChange = [this]{effectSelected();};

    //sliders and their labels    
    for(int i = 0; i < sliders.size(); i++)
    {
        addAndMakeVisible(sliders[i]);

        sliders[i].setColour(juce::Slider::ColourIds::thumbColourId, highlightColour);
        sliders[i].setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxLeft, false, 50, 20);
        
        addAndMakeVisible(sliderLabels[i]);
        sliderLabels[i].attachToComponent(&sliders[i], true);
    }

    //files
    juce::File appDataFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    juce::File subFolder = appDataFolder.getChildFile("duality");
    if(!subFolder.exists())
        subFolder.createDirectory();
    
    sourceFile = subFolder.getChildFile("source.wav");
    if(!sourceFile.exists())
        sourceFile.create();
    
    transformedFile = subFolder.getChildFile("transformed.wav");
    if(!transformedFile.exists())
        transformedFile.create();

    sourceWaveform.addChangeListener(this);
    transformedWaveform.addChangeListener(this);

    juce::MessageManager::callAsync([this]()
    {
        loadFilesIntoEditor();

        transformOnlyAttachment.emplace(processorRef.apvts, "transformOnly", transformOnlyToggle);
        effectAttachment.emplace(processorRef.apvts, "effect", effectList);
        modeAttachment.emplace(processorRef.apvts, "mode", modeList);

        sliderAttachment0.emplace(processorRef.apvts, "param_0", sliders[0]);
        sliderAttachment1.emplace(processorRef.apvts, "param_1", sliders[1]);
        sliderAttachment2.emplace(processorRef.apvts, "param_2", sliders[2]);
        sliderAttachment3.emplace(processorRef.apvts, "param_3", sliders[3]);

        modeList.setText(processorRef.transformMode);
        effectList.setText(processorRef.effect->getEffectName());
        handleSliders();
    });
}

DualityAudioProcessorEditor::~DualityAudioProcessorEditor()
{
    sourceWaveform.removeAllChangeListeners();
    transformedWaveform.removeAllChangeListeners();
}

//==============================================================================
void DualityAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(bgColour);
    const int stroke = 5;

    //source box
    g.setColour(highlightColour);
    g.drawRect(juce::Rectangle<int>(margin, 25+margin, getWidth() - 2*margin, 120), stroke);

        //paint source thumbnail
    juce::Rectangle<int> waveformRect (200, margin+35, 410-2*margin, 100);
    g.setColour(juce::Colours::white);
    if(sourceWaveform.getNumChannels() == 0)
        g.drawFittedText("empty file", waveformRect, juce::Justification::centred, 1);
    else
        sourceWaveform.drawChannels(g, waveformRect, 0.0, sourceWaveform.getTotalLength(), 1.0f);
        
    //transform box
    g.setColour(highlightColour);
    g.drawRect(juce::Rectangle<int>(margin, 190+margin, getWidth() - 2*margin, 220), stroke);
    g.drawLine(juce::Line<float>(getWidth()/2.0f, 175.0f+2*margin, getWidth()/2.0f, 386.0f+2*margin), stroke);

    g.setColour(drawColour);
    modeUI.draw(g);
    g.setColour(highlightColour);

    //output box
    g.drawRect(juce::Rectangle<int>(margin, 455+margin, getWidth() - 2*margin, 120), stroke);

        //paint transformed wavefrom
    waveformRect = juce::Rectangle<int>(200, 465+margin, 410-2*margin, 100);
    g.setColour(drawColour);
    if(transformedWaveform.getNumChannels() == 0)
        g.drawFittedText("empty file", waveformRect, juce::Justification::centred, 1);
    else
        transformedWaveform.drawChannels(g, waveformRect, 0.0, transformedWaveform.getTotalLength(), 1.0f);

}

void DualityAudioProcessorEditor::resized()
{
    
    //source
    sourceLabel.setBounds(margin, margin, 100, 20);

    openButton.setBounds(margin+10, margin+35, 180-margin, 45);

    //transform
    transformLabel.setBounds(margin, 165+margin, 100, 20);

    modeList.setBounds(margin+10, 200+margin, getWidth()/2 - 2*margin, 45);

    effectList.setBounds(getWidth()/2 + 10, 200+margin, getWidth()/2 - 2*margin, 45);

    for(int i = 0; i < sliders.size(); i++)
    {
        sliders[i].setBounds(getWidth()/2 + 70, 255+margin + i*35, getWidth()/2 - 2*margin - 70, 35);
    }

    transformButton.setBounds(margin+10, 375, 180-margin, 45);
    transformOnlyToggle.setBounds(200, 375, 100, 45);

    //output
    outputLabel.setBounds(margin, 430+margin, 100, 20);

    playButton.setBounds(margin+10, 465+margin, 77, 45);
    stopButton.setBounds(margin+93, 465+margin, 77, 45);
    saveButton.setBounds(margin+10, 520+margin, 77, 45);
    dragButton.setBounds(margin+93, 520+margin, 77, 45);
}

// MY STUFF
void DualityAudioProcessorEditor::openButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>("select file", juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode 
                      | juce::FileBrowserComponent::canSelectFiles;
    
    fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser)
    {
        juce::File chosen = chooser.getResult();

        if(chosen != juce::File{})
        {
            //deal with audioSource
            chosen.copyFileTo(sourceFile);
            sourceFile.copyFileTo(transformedFile);
            juce::MessageManager::callAsync ([this]{loadFilesIntoEditor();});
        }
    });
}

void DualityAudioProcessorEditor::playButtonClicked()
{
    processorRef.changeState(DualityAudioProcessor::TransportState::Starting);
    openButton.setEnabled(false);
    stopButton.setEnabled(true);
    transformButton.setEnabled(false);
    playButton.setEnabled(false);
}

void DualityAudioProcessorEditor::stopButtonClicked()
{
    processorRef.changeState(DualityAudioProcessor::TransportState::Stopping);
    openButton.setEnabled(true);
    transformButton.setEnabled(true);
    playButton.setEnabled(true);
}

void DualityAudioProcessorEditor::transformButtonClicked()
{
    processorRef.process(sourceFile, transformedFile);
    loadFilesIntoEditor();
}

void DualityAudioProcessorEditor::toggleClicked()
{
    bool effectsEnabled = !transformOnlyToggle.getToggleState();
    effectList.setEnabled(effectsEnabled);
    for(int i = 0; i < sliders.size(); i++)
        sliders[i].setEnabled(effectsEnabled);

    processorRef.transformOnly = !effectsEnabled;
}

void DualityAudioProcessorEditor::saveButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>("save file", juce::File{}, "*.wav");

    auto fileChooserFlags = juce::FileBrowserComponent::saveMode |
                            juce::FileBrowserComponent::canSelectFiles |
                            juce::FileBrowserComponent::warnAboutOverwriting;
    
    fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
    {
        juce::File toSave = chooser.getResult();

        if(toSave != juce::File{})
        {
            transformedFile.copyFileTo(toSave);
        }
    });
}

void DualityAudioProcessorEditor::handleSliders()
{
    EffectSlot *e = processorRef.effect;
    for(int i = 0; i < sliderLabels.size(); i++)
    {
        sliders[i].range = e->parameterRanges[i];
        sliders[i].skew = e->parameterSkews[i];
        sliders[i].updateText();
        sliderLabels[i].setText(e->parameterNames[i], juce::NotificationType::dontSendNotification);
    }
}

void DualityAudioProcessorEditor::effectSelected()
{
    if(!isFirstLoad)
    {
        processorRef.setEffect(effectList.getText());
        processorRef.loadDefaultParameters();
        
        handleSliders();
    }
    
    isFirstLoad = false;
}

void DualityAudioProcessorEditor::modeSelected()
{
    processorRef.transformMode = modeList.getText();
    modeUI.setMode(processorRef.transformMode);
    repaint();
}

bool DualityAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray &files)
{
    for(juce::String file : files)
    {
        if(file.contains(".wav"))
            return true;
    }

    return false;
}

void DualityAudioProcessorEditor::filesDropped(const juce::StringArray &files, int x, int y)
{
    juce::ignoreUnused(x, y);

    playButton.setEnabled(true);
    for(juce::String file : files)
    {
        if(isInterestedInFileDrag(files))
        {
            //deal with source
            juce::File{file}.copyFileTo(sourceFile);
            sourceFile.copyFileTo(transformedFile);
            juce::MessageManager::callAsync ([this]{loadFilesIntoEditor();});
        }
    }
}

void DualityAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    ignoreUnused(source);
    repaint();
}

void DualityAudioProcessorEditor::loadFilesIntoEditor()
{
    processorRef.loadFile(transformedFile);

    thumbnailCache.clear();
    sourceWaveform.clear();
    transformedWaveform.clear();

    sourceWaveform.setSource(new juce::FileInputSource(sourceFile));
    transformedWaveform.setSource(new juce::FileInputSource(transformedFile));
}
