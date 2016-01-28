#ifndef _ANNTRAINDATA_H_
#define _ANNTRAINDATA_H_

#include "Common.h"

class ANNTrainData
{
public:
    ANNTrainData();

    virtual ~ANNTrainData();

    void Init(unsigned int sampleCount, unsigned int inputCount, unsigned int outputCount);
    
    unsigned int GetSampleCount() const;

    unsigned int GetInputCount() const;

    unsigned int GetOutputCount() const;

    void SetInput(unsigned int sample, unsigned int index, Real value);

    Real GetInput(unsigned int sample, unsigned int index) const;

    void SetOutput(unsigned int sample, unsigned int index, Real value);
    
    Real GetOutput(unsigned int sample, unsigned int index) const;

private:
    unsigned int mInputCount;
    unsigned int mOutputCount;

    unsigned int mSampleCount;

    std::vector<Real> mOutputs;
    std::vector<Real> mInputs;
};

#endif