#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DualityAudioProcessor::DualityAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                       effectParameters(4),
                       apvts(*this, nullptr, "Parameters", createParameterLayout())                      
{
    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);
}

DualityAudioProcessor::~DualityAudioProcessor()
{
    delete effect;
}

//==============================================================================
const juce::String DualityAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DualityAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DualityAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DualityAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DualityAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DualityAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DualityAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DualityAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String DualityAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void DualityAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void DualityAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    transportSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void DualityAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    transportSource.releaseResources();
}

bool DualityAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void DualityAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if(state == TransportState::Playing)
    {
        transportSource.getNextAudioBlock(juce::AudioSourceChannelInfo(&buffer, 0, buffer.getNumSamples()));
        if(transportSource.hasStreamFinished())
        {
            changeState(TransportState::Stopped);
            sendChangeMessage();
        }
    }
}

//==============================================================================
bool DualityAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DualityAudioProcessor::createEditor()
{
    return new DualityAudioProcessorEditor (*this);
}

//==============================================================================
void DualityAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos (destData, true);
    apvts.state.writeToStream(mos);
}

void DualityAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if(tree.isValid())
    {
        apvts.replaceState(tree);
        setEffect(EffectSlot::effectNames[(int)*apvts.getRawParameterValue("effect")]);
        loadParameters();
    }
    else
    {
        loadDefaultParameters();
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout DualityAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterChoice>("mode", "mode", modeNames, 0));
    layout.add(std::make_unique<juce::AudioParameterBool>("transformOnly", "transformOnly", false));
    layout.add(std::make_unique<juce::AudioParameterChoice>("effect", "effect", EffectSlot::effectNames, 0));

    for(int i = 0; i < effectParameters.size(); i++)
    {
        juce::String name ("param_" + std::to_string(i));
        layout.add(std::make_unique<juce::AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.0f));
    }

    return layout;
}

// MY STUFF
void DualityAudioProcessor::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;
        switch (state)
        {
            case Stopped:
                transportSource.setPosition(0.0);
                break;
            case Starting:
                transportSource.start();
                break;
            case Stopping:
                transportSource.stop();
                break;
        }
    }
}

void DualityAudioProcessor::loadFile(juce::File &filename)
{
    auto* reader = formatManager.createReaderFor(filename);

    if(reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());

        //set sample rate for fx processing
        EffectSlot::setSampleRate(reader->sampleRate);
    }
}

void DualityAudioProcessor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if(source == &transportSource)
    {
        if(transportSource.isPlaying())
        {
            changeState(Playing);
        }
        else
        {
            changeState(Stopped);
        }
    }
}

void DualityAudioProcessor::process(const juce::File &inputFile, juce::File &transformedFile)
{
    loadParameters();

    juce::AudioFormatReader* reader = formatManager.createReaderFor(inputFile);
    juce::AudioBuffer<float> data = juce::AudioBuffer<float>(reader->numChannels, static_cast<int>(reader->lengthInSamples));
    juce::File outputFile = juce::File::createTempFile(".wav");

    if(reader->read(&data, 0, static_cast<int>(reader->lengthInSamples), 0, true, true))
    {
        transform(data);
        data.applyGain(1.0f / data.getMagnitude(0, data.getNumSamples()));
        if(!transformOnly)
        {
            applyEffects(data);
            itransform(data);
            data.applyGain(1.0f / data.getMagnitude(0, data.getNumSamples()));
        }

        juce::WavAudioFormat wavFormat;
        std::unique_ptr<juce::FileOutputStream> outStream(outputFile.createOutputStream());
        if(outStream == nullptr){return;}
        
        std::unique_ptr<juce::AudioFormatWriter> writer(wavFormat.createWriterFor(
                                                                    outStream.get(),
                                                                    reader->sampleRate,
                                                                    reader->numChannels,
                                                                    reader->bitsPerSample,
                                                                    {},
                                                                    0));
        if(writer == nullptr){return;}

        outStream.release();
        writer->writeFromAudioSampleBuffer(data, 0, data.getNumSamples());
    }

    outputFile.copyFileTo(transformedFile);
    outputFile.deleteFile();

    delete reader;
    return;
}

void DualityAudioProcessor::ifft(juce::AudioBuffer<float> &data)
{
    //original length in samples
    int length = data.getNumSamples();

    //create fft object with 2^order >= data length
    int order = static_cast<int>(std::ceil(std::log(data.getNumSamples()) / std::log(2)));
    juce::dsp::FFT fourier(order);

    //pad data so data length = 2 * 2^order
    data.setSize(data.getNumChannels(), 2<<order, true, true, false);
    for(int i = 0; i < data.getNumChannels(); i++)
    {
        fourier.performRealOnlyInverseTransform(data.getWritePointer(i));
    }

    //delete last half of data
    data.setSize(data.getNumChannels(), length, true, true, false);

    return;
}

void DualityAudioProcessor::transform(juce::AudioBuffer<float> &data)
{
    if(transformMode == "flip f=t")
        ifft(data);
    else if(transformMode == "+pi/2")
    {
        ifft(data);
        data.reverse(0, data.getNumSamples());
    }
    else if(transformMode == "-pi/2")
    {
        data.reverse(0, data.getNumSamples());
        ifft(data);
    }
    else if(transformMode == "+pi")
    {
        ifft(data);
        data.reverse(0, data.getNumSamples());
        ifft(data);
        data.reverse(0, data.getNumSamples());
    }
    else if(transformMode == "flip f=fmax-t")
    {
        data.reverse(0, data.getNumSamples());
        ifft(data);
        data.reverse(0, data.getNumSamples());
    }

}

void DualityAudioProcessor::itransform(juce::AudioBuffer<float> &data)
{
    if(transformMode == "flip f=t")
        ifft(data);
    else if(transformMode == "+pi/2")
    {
        data.reverse(0, data.getNumSamples());
        ifft(data);
    }
    else if(transformMode == "-pi/2")
    {
        ifft(data);
        data.reverse(0, data.getNumSamples());
    }
    else if(transformMode == "+pi")
    {
        data.reverse(0, data.getNumSamples());
        ifft(data);
        data.reverse(0, data.getNumSamples());
        ifft(data);
    }
    else if(transformMode == "flip f=fmax-t")
    {
        data.reverse(0, data.getNumSamples());
        ifft(data);
        data.reverse(0, data.getNumSamples());
    }
}

void DualityAudioProcessor::setEffect(juce::String effectType)
{    
    if(effect != nullptr)
        delete effect;

    if(effectType == EffectSlot::effectNames[0])
    {
        effect = new Distortion();
    }
    else if(effectType == EffectSlot::effectNames[1])
    {
        effect = new NoiseConvolution();
    }
    else if(effectType == EffectSlot::effectNames[2])
    {
        effect = new Pad();
    }
    else if(effectType == EffectSlot::effectNames[3])
    {
        effect = new Delay();
    }
    else if(effectType == EffectSlot::effectNames[4])
    {
        effect = new SampleSkew();
    }
}

void DualityAudioProcessor::loadDefaultParameters()
{
    for(int i = 0; i < effectParameters.size(); i++)
    {
        juce::String name ("param_" + std::to_string(i));
        float value = effect->invSkew(effect->parameterDefaults[i], i);
        apvts.getParameter(name)->setValueNotifyingHost(value);
    }
}

void DualityAudioProcessor::addTreeChild(juce::ValueTree &parent,
                                            const juce::Identifier &childType,
                                            const juce::Identifier &propertyName,
                                            const juce::var &propertyValue)
{
    juce::ValueTree child(childType);
    child.setProperty(propertyName, propertyValue, nullptr);
    parent.addChild(child, -1, nullptr);
}

void DualityAudioProcessor::loadParameters()
{
    transformMode = modeNames[(int)*apvts.getRawParameterValue("mode")];
    transformOnly = *apvts.getRawParameterValue("transformOnly");

    for(int i = 0; i < effectParameters.size(); i++)
    {
        juce::String name ("param_" + std::to_string(i));
        effectParameters[i] = *apvts.getRawParameterValue(name);
    }
}

void DualityAudioProcessor::applyEffects(juce::AudioBuffer<float> &data)
{
    if(effect != nullptr)
        effect->apply(data, effectParameters);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DualityAudioProcessor();
}
