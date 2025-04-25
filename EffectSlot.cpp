#include "EffectSlot.h"

EffectSlot::EffectSlot()
{
    
}

void EffectSlot::setSampleRate(double sr)
{
    sampleRate = sr;
}

float EffectSlot::skewFunction(float input, int index)
{
    return parameterRanges[index][0] + (parameterRanges[index][1] - parameterRanges[index][0])*pow(input, 1/parameterSkews[index]);
}

float EffectSlot::invSkew(float input, int index)
{
    return pow((input - parameterRanges[index][0])/(parameterRanges[index][1]-parameterRanges[index][0]), parameterSkews[index]);
}
