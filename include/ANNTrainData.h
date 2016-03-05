/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
 */

#ifndef _ANNTRAINDATA_H_
#define _ANNTRAINDATA_H_

#include "Common.h"

/*! \class ANNTrainData
 *  \brief Artificial Neural Network train data set.
 */
class ANNTrainData
{
public:
    /*! \brief Default constructor.
     */
    ANNTrainData();

    /*! \brief Virtual destructor.
     */
    virtual ~ANNTrainData();

    /*! \brief Allocate memory needed for samples,
     *  input and output neurons, respectively.
     *
     *  \param sampleCount The number of samples to allocate.
     *  \param inputCount The number of input neurons each sample should have.
     *  \param outputCount The number of output neurons each sample should have.
     */
    void Init(unsigned int sampleCount, unsigned int inputCount, unsigned int outputCount);
    
    /*! \brief Get the number of samples.
     *
     *  \return The number of samples allocated.
     */
    unsigned int GetSampleCount() const;

    /*! \brief Get the number of input neurons.
     *
     *  \return The number of input neurons used in each sample.
     */
    unsigned int GetInputCount() const;

    /*! \brief Get the number of output neurons.
     *
     *  \return The number of output neurons used in each sample.
     */
    unsigned int GetOutputCount() const;

    /*! \brief Set the value of an allocated input neuron related to a sample index.
     *
     *  \param sample The sample index.
     *  \param index The input neuron index.
     *  \param value The new value of the neuron.
     */
    void SetInput(unsigned int sample, unsigned int index, Real value);
    
    /*! \brief Get the value of an allocated input neuron related to a sample index.
     *
     *  \param sample The sample index.
     *  \param index The input neuron index.
     *
     *  \return The value of the input neuron.
     */
    Real GetInput(unsigned int sample, unsigned int index) const;
    
    /*! \brief Set the value of an allocated output neuron related to a sample index.
     *
     *  \param sample The sample index.
     *  \param index The output neuron index.
     *  \param value The new value of the neuron.
     */
    void SetOutput(unsigned int sample, unsigned int index, Real value);
    
    /*! \brief Get the value of an allocated output neuron related to a sample index.
     *
     *  \param sample The sample index.
     *  \param index The output neuron index.
     *
     *  \return The value of the output neuron.
     */
    Real GetOutput(unsigned int sample, unsigned int index) const;

private:
    unsigned int mInputCount;
    unsigned int mOutputCount;

    unsigned int mSampleCount;

    std::vector<Real> mOutputs;
    std::vector<Real> mInputs;
};

#endif