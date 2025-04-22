#include "NoiseConvolution.h"

NoiseConvolution::NoiseConvolution()
{
    parameterNames = {"decay", "order", "width", "dry/wet"};
    parameterDefaults = {0.01f, 10.0f, 1.0f, 1.0f};
    parameterRanges = {{0.0f, 1.0f}, {0.0f, 100.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}};
    parameterSkews = {0.2f, 0.5f, 1.0f, 1.0f};
}

void NoiseConvolution::apply(juce::AudioBuffer<float> &dry, std::vector<float> parameters)
{   
    float decay = skewFunction(parameters[NoiseConvolution::decay], NoiseConvolution::decay);
    float order = skewFunction(parameters[NoiseConvolution::order], NoiseConvolution::order);
    float width = parameters[NoiseConvolution::width];
    float drywet = parameters[NoiseConvolution::drywet];

    juce::AudioBuffer<float> wet (dry);
    const int numSamples = wet.getNumSamples();

    //perform convolution
    juce::AudioBuffer<float> ir = createIR(static_cast<int>(std::floor(decay * numSamples)), order);

    juce::dsp::Convolution conv;
    conv.loadImpulseResponse(std::move(ir),
                             sampleRate,
                             juce::dsp::Convolution::Stereo::yes,
                             juce::dsp::Convolution::Trim::yes,
                             juce::dsp::Convolution::Normalise::no);
                             
    
    conv.prepare({sampleRate, static_cast<unsigned>(numSamples), 2u});

    juce::dsp::AudioBlock<float> block(wet);
    juce::dsp::ProcessContextReplacing context(block);
    conv.process(context);

    //width
    float *left = wet.getWritePointer(0);
    float *right = left;
    if(wet.getNumChannels() > 1)
        right = wet.getWritePointer(1);

    for(int i= 0; i < numSamples; i++)
    {
        float mid = (left[i] + right[i])/2;
        float side = (left[i] - right[i])/2;

        left[i] = mid + width*side;
        right[i] = mid - width*side;
    }
    
    //dry wet
    dry.applyGain(1.0f - drywet);
    wet.applyGain(drywet / wet.getMagnitude(0, numSamples));


    for(int channel = 0; channel < dry.getNumChannels(); channel++)
        dry.addFrom(channel, 0, wet, channel, 0, numSamples);

}

juce::AudioBuffer<float> NoiseConvolution::createIR(int numSamples, float order)
{
    juce::AudioBuffer<float> ir(2, numSamples);
    std::mt19937 mt(static_cast<unsigned int>(std::time(nullptr)));
    std::normal_distribution<float> gaussian(0.0f, 0.33f); //mean, stddev

    for(int channel = 0; channel < ir.getNumChannels(); channel++)
    {
        float *write = ir.getWritePointer(channel);
        for(int i = 0; i < numSamples; i++)
        {
            *(write + i) = gaussian(mt) * pow(static_cast<float>(numSamples-i)/numSamples, order);
        }
    }
    
    return ir;
}

juce::String NoiseConvolution::getEffectName()
{
    return EffectSlot::effectNames[1];
}

