/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
 */

#ifndef _ANN_H_
#define _ANN_H_

#include "Common.h"

#include "ANNTrainData.h"

/*! \class ANN
 *  \brief Artificial Feed-Forward Neural Network.
 *  
 *  Supported training algorithms:
 *  BPROP (backpropagation),
 *  BPROP_BATCH (batch backpropagation) and
 *  RPROP (resilient backpropagation).
 */
class ANN
{
public:
    enum class TrainingAlgorithm
    {
        BPROP = 0,
        BPROP_BATCH,
        RPROP
    };

protected:
    struct Neuron;
    struct Synapse;

    struct Neuron
    {
        Real value;
        Real delta;

        Synapse* beginSynapse;
        Synapse* endSynapse;
    };

    struct Layer
    {
        Neuron* beginNeuron;
        Neuron* endNeuron;
    };

    struct Synapse 
    {
        Real weight;
        Real gradient;
        Real previousGradient;
        Real updateValue;
        Real deltaWeight;
    };

public:
    /*! \brief Default constructor.
     */
    ANN();
    
    /*! \brief Virtual destructor.
     */
    virtual ~ANN();

    /*! \brief Initialize the network with default values.
     */
    void Init();
    
    /*! \brief Set the learning rate of the BPROP algorithm.
     *
     *  \param The BPROP learning rate.
     */
    void SetLearningRate(Real rate);
    
    /*! \brief Get the learning rate of the BPROP algorithm.
     *
     *  \return The BPROP learning rate.
     */
    Real GetLearningRate() const;
    
    /*! \brief Set the stopping RMSE threshold of the algorithms.
     *
     *  \param threshold The RMSE stopping threshold.
     */
    void SetErrorThreshold(Real threshold);
    
    /*! \brief Get the stopping RMSE threshold of the algorithms.
     *
     *  \return RMSE The stopping threshold.
     */
    Real GetErrorThreshold() const;

    /*! \brief Set the maximum number of training iterations.
     *
     *  \param epochs The number of epochs (iterations).
     */
    void SetMaxEpochs(unsigned int epochs);
    
    /*! \brief Get the maximum number of training iterations.
     *
     *  \return The number of epochs (iterations).
     */
    unsigned int GetMaxEpochs() const;
    
    /*! \brief Set the main training algorithm.
     *
     *  Possible algorithms are:
     *  BPROP (backpropagation),
     *  BPROP_BATCH (batch backpropagation) and
     *  RPROP (resilient backpropagation).
     *
     *  \param algorithm The training algorithm.
     */
    void SetTrainingAlgorithm(TrainingAlgorithm algorithm);
    
    /*! \brief Get the main training algorithm.
     *
     *  \return The training algorithm.
     *
     *  \see SetTrainingAlgorithm
     */
    TrainingAlgorithm GetTrainingAlgorithm() const;
    
    /*! \brief Build the network structure.
     */
    void Build();
    
    /*! \brief Print the built network structure.
     */
    void PrintStructure();
    
    /*! \brief Add a new layer of neurons to the structure.
     *
     *  \param neuronCount The number of neurons in the layer.
     */
    void AddLayer(unsigned int neuronCount);
    
    /*! \brief Train the network.
     *
     *  \param data The data set used for training.
     */
    void Train(const ANNTrainData& data);

    /*! \brief Set the value of an input neuron.
     *
     *  \param index The neuron index.
     *  \param value The value of the neuron.
     */
    void SetInput(unsigned int index, Real value);
    
    /*! \brief Get the value of an input neuron.
     *
     *  \param index The neuron index.
     *
     *  \return The value of the neuron.
     */
    Real GetInput(unsigned int index) const;
    
    /*! \brief Get the number of input neurons.
     *
     *  \return The number of neurons in the input layer.
     */
    unsigned int GetInputCount() const;
    
    /*! \brief Set the value of an output neuron.
     *
     *  \param index Neuron index.
     *  \param value Value of the neuron.
     */
    void SetOutput(unsigned int index, Real value);
    
    /*! \brief Get the value of an output neuron.
     *
     *  \param index The neuron index.
     *
     *  \return The value of the neuron.
     */
    Real GetOutput(unsigned int index) const;
    
    /*! \brief Get the number of output neurons.
     *
     *  \return The number of neurons in the output layer.
     */
    unsigned int GetOutputCount() const;
    
    /*! \brief Run the network.
     *
     *  This runs the network with already set input values.
     *  The output values can be fetched via GetOutput().
     *
     *  \see SetInput()
     *  \see GetOutput()
     */
    void Run();
    
private:
    /*! \brief Print training progress.
     */
    void PrintProgress();
    
    /*! \brief Calculate RMS error of the network over training samples.
     *
     *  \param data Training data.
     */
    void CalculateRMSE(const ANNTrainData& data);
    
    /*! \brief Weight update of the batch BPROP algorithm.
     */
    void UpdateWeightsBpropBatch();
    
    /*! \brief Weight update of the BPROP algorithm.
     */
    void UpdateWeightsBprop();
    
    /*! \brief Weight update of the RPROP algorithm.
     */
    void UpdateWeightsRprop();
    
    /*! \brief Calculate output layer deltas.
     *
     *  \param data The data set used for training.
     *  \param sample A sample index in the training data set.
     */
    void CalculateOutputDeltas(const ANNTrainData& data, unsigned int sample);
    
    /*! \brief Calculate the deltas of the hidden layers.
     */
    void CalculateHiddenDeltas();
    
    /*! \brief Calculate final gradients.
     */
    void AccumulateGradients();

private:
    std::vector<Neuron> mNeurons;
    std::vector<Synapse> mSynapses;
    
    std::vector<Layer> mLayers;
    
    std::vector<unsigned int> mLayerSizes;
    std::vector<unsigned int> mSynapseCounts;
    
    TrainingAlgorithm mTrainingAlgorithm;

    Real mRMSE; 

    unsigned int mEpoch;

    Real mLearningRate;

    unsigned int mMaxEpochs;

    Real mErrorThreshold;
};

#endif