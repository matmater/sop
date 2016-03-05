/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
 */

#include "ANNTrainData.h"

ANNTrainData::ANNTrainData()
{

}

ANNTrainData::~ANNTrainData()
{

}

void ANNTrainData::Init(unsigned int sampleCount, unsigned int inputCount, unsigned int outputCount)
{
    mInputs.clear();
    mOutputs.clear();

    mSampleCount = sampleCount;

    mInputCount = inputCount;
    mOutputCount = outputCount;
        
    mInputs.resize(sampleCount * inputCount);
    mOutputs.resize(sampleCount * outputCount);
}
    
unsigned int ANNTrainData::GetSampleCount() const
{
    return mSampleCount;
}

unsigned int ANNTrainData::GetInputCount() const
{
    return mInputCount;
}

unsigned int ANNTrainData::GetOutputCount() const
{
    return mOutputCount;
}

void ANNTrainData::SetInput(unsigned int sample, unsigned int index, Real value)
{
    mInputs[sample * mInputCount + index] = value;
}

Real ANNTrainData::GetInput(unsigned int sample, unsigned int index) const
{
    return mInputs[sample * mInputCount + index];
}

void ANNTrainData::SetOutput(unsigned int sample, unsigned int index, Real value)
{
    mOutputs[sample * mOutputCount + index] = value;
}
    
Real ANNTrainData::GetOutput(unsigned int sample, unsigned int index) const
{
    return mOutputs[sample * mOutputCount + index];
}
