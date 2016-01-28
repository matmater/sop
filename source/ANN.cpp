#include "ANN.h"

ANN::ANN()
{
    Init();
}

ANN::~ANN()
{

}

void ANN::Init()
{
    mNeurons.clear();
    mSynapses.clear();

    mLayers.clear();

    mLayerSizes.clear();
    mSynapseCounts.clear();

    mRMSE = -1.0f;

    mLearningRate = 0.05f;

    mTrainingAlgorithm = TrainingAlgorithm::BPROP_BATCH;

    mErrorThreshold = 1.0e-3f;
    mMaxEpochs = 10000;
}
    
void ANN::SetLearningRate(Real rate)
{
    mLearningRate = rate;
}

Real ANN::GetLearningRate() const
{
    return mLearningRate;
}

void ANN::SetErrorThreshold(Real threshold)
{
    mErrorThreshold = threshold;
}

Real ANN::GetErrorThreshold() const
{
    return mErrorThreshold;
}

void ANN::SetMaxEpochs(unsigned int epochs)
{
    mMaxEpochs = epochs;
}

unsigned int ANN::GetMaxEpochs() const
{
    return mMaxEpochs;
}

void ANN::SetTrainingAlgorithm(ANN::TrainingAlgorithm algorithm)
{
    mTrainingAlgorithm = algorithm;
}

ANN::TrainingAlgorithm ANN::GetTrainingAlgorithm() const
{
    return mTrainingAlgorithm;
}
    
void ANN::Build()
{
    DEBUG_TRACE("Building network...");

    unsigned int neuronCount = 0;
    unsigned int synapseCount = 0;
        
    DEBUG_TRACE("Traversing layers...");
        
    DEBUG_TRACE("Layer: " << 0);
        
    DEBUG_TRACE("Neurons: " << mLayerSizes[0]);
        
    // Hidden & output neurons.
    for (unsigned int l = 1; l < mLayerSizes.size(); l++)
    {
        DEBUG_TRACE("Layer: " << l)

        DEBUG_TRACE("Neurons: " << mLayerSizes[l] << std::endl
            << "Incoming bias synapses: " << mLayerSizes[l] << std::endl
            << "Incoming synapses: " << mLayerSizes[l - 1] * mLayerSizes[l] << std::endl
            << "Previous layer bias neurons: " << 1);

        synapseCount += mLayerSizes[l - 1] * mLayerSizes[l];
            
        // Bias neuron.
        mLayerSizes[l - 1] += 1;

        // Bias synapses.
        synapseCount += mLayerSizes[l];

        // Neuron count.
        neuronCount += mLayerSizes[l - 1];
    }

    neuronCount += mLayerSizes[mLayerSizes.size() - 1];
    
    DEBUG_TRACE("All layers traversed.");
    DEBUG_TRACE("Total neurons: " << neuronCount << std::endl
        << ">>> Bias neurons: " << mLayerSizes.size() - 1 << std::endl
        << "Total synapses: " << synapseCount);
        
    DEBUG_TRACE("Allocating memory...");

    mNeurons.resize(neuronCount);
    mSynapses.resize(synapseCount);
    
    DEBUG_TRACE("Initializing structures...");

    Neuron* neuron = &mNeurons[0];
    Synapse* synapse = &mSynapses[0];

    mLayers.resize(mLayerSizes.size());
        
    // Set the begin & end neurons of each layer.
    for (unsigned int l = 0; l < mLayerSizes.size(); ++l)
    {
        Layer* layer = &mLayers[l];

        layer->beginNeuron = neuron;
        layer->endNeuron = neuron + mLayerSizes[l];

        neuron += mLayerSizes[l];
    }
        
    // Set the begin & end synapses of each layer's neurons,
    // excluding the last output layer.
    for (unsigned int l = 1; l < mLayerSizes.size(); ++l)
    {
        Layer* left = &mLayers[l - 1];
        Layer* right = &mLayers[l];
            
        Neuron* leftNeuron = left->beginNeuron;
        Neuron* leftEndNeuron = left->endNeuron;

        // Go through the neurons of the left layer.
        for (; leftNeuron != leftEndNeuron; ++leftNeuron)
        {
            leftNeuron->beginSynapse = synapse;
                
            // Last layer -> connect to all neurons (last layer does not have
            // a bias neuron).
            if (l == mLayerSizes.size() - 1)
            {
                synapse += mLayerSizes[l];
            }
                
            // Not last layer -> connect to all neurons except the bias neuron,
            // which is always the last neuron.
            else
            {
                synapse += mLayerSizes[l] - 1;
            }

            leftNeuron->endSynapse = synapse;
        }
    }

    DEBUG_TRACE("Building network done.");
}

void ANN::PrintStructure()
{
    Neuron* firstNeuron = &mNeurons[0];

    Synapse* firstSynapse = &mSynapses[0];

    for (unsigned int l = 1; l < mLayers.size(); ++l)
    {
        Layer* layer = &mLayers[l - 1];

        Layer* right = &mLayers[l];

        std::cout << std::endl << "Layer: " << l - 1 << std::endl;

        Neuron* neuron = layer->beginNeuron;

        for (; neuron != layer->endNeuron; ++neuron)
        {
            std::cout << neuron - firstNeuron << ":" << neuron->value << " ";
        }
            
        std::cout << std::endl;
            
        neuron = layer->beginNeuron;

        Neuron* rightBeginNeuron = right->beginNeuron;

        for (; neuron != layer->endNeuron; ++neuron)
        {
            Synapse* synapse = neuron->beginSynapse;
                
            Neuron* outNeuron = rightBeginNeuron;

            for (; synapse != neuron->endSynapse; ++synapse, ++outNeuron)
            {
                std::cout << synapse - firstSynapse << ":"
                    << neuron - firstNeuron << "->"
                    << outNeuron - firstNeuron << " ";
            }
        }

        std::cout << std::endl;
    }
}

void ANN::AddLayer(unsigned int neuronCount)
{
    mLayerSizes.push_back(neuronCount);
}

void ANN::Train(const ANNTrainData& data)
{
    std::cout << "-- Training --" << std::endl;
        
    std::cout << "Error Threshold: " << mErrorThreshold << std::endl;
    std::cout << "Max Epochs: " << mMaxEpochs << std::endl;

    // Initialize synapse weights.
    for (Synapse& synapse : mSynapses)
    {
        synapse.weight = RandomGaussian() * 0.05f;
        synapse.gradient = 0.0f;
        synapse.previousGradient = 0.0f;
        synapse.updateValue = 0.1f;
        synapse.deltaWeight = 0.0f;
    }

    for (mEpoch = 0; mEpoch < mMaxEpochs; ++mEpoch)
    {
        for (Synapse& synapse : mSynapses)
        {
            synapse.gradient = 0.0f;
        }

        for (unsigned int s = 0; s < data.GetSampleCount(); s++)
        {
            for (unsigned int i = 0; i < data.GetInputCount(); i++)
            {
                SetInput(i, data.GetInput(s, i));
            }

            Run();

            switch (mTrainingAlgorithm)
            {
            case TrainingAlgorithm::BPROP:
                CalculateOutputDeltas(data, s);
                CalculateHiddenDeltas();
                UpdateWeightsBprop();
                break;
                    
            case TrainingAlgorithm::BPROP_BATCH:
            case TrainingAlgorithm::RPROP:
                CalculateOutputDeltas(data, s);
                CalculateHiddenDeltas();
                AccumulateGradients();
            }
        }
            
        switch (mTrainingAlgorithm)
        {
        case TrainingAlgorithm::BPROP_BATCH:
            UpdateWeightsBpropBatch();
            break;

        case TrainingAlgorithm::RPROP:
            UpdateWeightsRprop();
        }

        CalculateRMSE(data);
            
        PrintProgress();
            
        if (mRMSE <= mErrorThreshold)
        {
            break;
        }
    }
}

void ANN::PrintProgress()
{
    std::cout << "Epoch: " << mEpoch << ", RMSE: " << mRMSE << std::endl;
}
    
void ANN::CalculateRMSE(const ANNTrainData& data)
{
    mRMSE = 0.0f;

    for (unsigned int s = 0; s < data.GetSampleCount(); s++)
    {
        for (unsigned int i = 0; i < data.GetInputCount(); i++)
        {
            SetInput(i, data.GetInput(s, i));
        }

        Run();

        Layer* outputLayer = &mLayers[mLayers.size() - 1];

        Real desired, error;

        Neuron* outputNeuron = outputLayer->beginNeuron;

        Real tmp = 0.0f;

        for (; outputNeuron != outputLayer->endNeuron; ++outputNeuron)
        {
            desired = data.GetOutput(s, outputNeuron - outputLayer->beginNeuron);
                
            error = outputNeuron->value - desired;

            mRMSE += error * error;
        }

        mRMSE += std::sqrt(tmp / static_cast<Real>(mLayerSizes.back()));
    }
}

void ANN::UpdateWeightsBpropBatch()
{
    Synapse* synapse = &mSynapses[0];
    Synapse* endSynapse = synapse + mSynapses.size();

    Real t = mLearningRate;

    for (; synapse != endSynapse; ++synapse)
    {
        synapse->weight -= t * synapse->gradient;
    }
}

void ANN::UpdateWeightsBprop()
{
    // E_ij = delta_j * out_i
    // delta_w = - learning_rate * E_ij

    for (unsigned int l = 1; l < mLayers.size(); ++l)
    {
        Layer* left = &mLayers[l - 1];
        Layer* right = &mLayers[l];

        Neuron* leftBeginNeuron = left->beginNeuron;
        Neuron* leftEndNeuron = left->endNeuron;
            
        Neuron* rightBeginNeuron = right->beginNeuron;
        Neuron* rightEndNeuron = right->endNeuron;

        Neuron* leftNeuron = leftBeginNeuron;

        for (; leftNeuron != leftEndNeuron; ++leftNeuron)
        {
            Synapse* synapse = leftNeuron->beginSynapse;
            Synapse* endSynapse = leftNeuron->endSynapse;

            Neuron* rightNeuron = rightBeginNeuron;

            Real t = mLearningRate * leftNeuron->value;

            for (; synapse != endSynapse; ++synapse, ++rightNeuron)
            {
                assert(rightNeuron != rightEndNeuron
                    && "UpdateWeights(): Invalid memory address.");
                    
                synapse->weight -= t * rightNeuron->delta;
            }
        }
    }
}

void ANN::UpdateWeightsRprop()
{
    // Martin Riedmiller. Rprop – Description and Implementation Details. Technical report, 1994.
    // http://www.inf.fu-berlin.de/lehre/WS06/Musterererkennung/Paper/rprop.pdf

    Synapse* synapse = &mSynapses[0];
    Synapse* synapsesEnd = synapse + mSynapses.size();

    Real eta_plus = 1.2f;
    Real eta_minus = 0.1f;

    Real delta_min = 1.0e-6f;
    Real delta_max = 50.0f;

    for (; synapse != synapsesEnd; ++synapse)
    {
        int E_prev_E_sign = Sign(synapse->gradient * synapse->previousGradient);

        if (E_prev_E_sign > 0)
        {
            synapse->updateValue = Min(synapse->updateValue * eta_plus, delta_max);
            synapse->deltaWeight = -Sign(synapse->gradient) * synapse->updateValue;
            synapse->weight += synapse->deltaWeight;
            synapse->previousGradient = synapse->gradient;
        }

        else if (E_prev_E_sign < 0)
        {
            synapse->updateValue = Max(synapse->updateValue * eta_minus, delta_min);
            synapse->previousGradient = 0.0f;
        }

        else if (E_prev_E_sign == 0)
        {
            synapse->deltaWeight = -Sign(synapse->gradient) * synapse->updateValue;
            synapse->weight += synapse->deltaWeight;
            synapse->previousGradient = synapse->gradient;
        }
    }
}

void ANN::CalculateOutputDeltas(const ANNTrainData& data, unsigned int sample)
{
    // delta_j = (out_j - target_j) * f'(out_j)

    Layer* layer = &mLayers[mLayers.size() - 1];

    Real desired, error;

    Neuron* neuron = layer->beginNeuron;

    for (; neuron != layer->endNeuron; ++neuron)
    {
        desired = data.GetOutput(sample, neuron - layer->beginNeuron);
            
        error = neuron->value - desired;

        neuron->delta = error * SigmoidDelta(neuron->value);
    }
}
    
void ANN::CalculateHiddenDeltas()
{
    // delta_j = sum(delta_l * weight_ij) * f'(out_j)

    for (unsigned int l = mLayers.size() - 2; l > 0; --l)
    {
        Layer* left = &mLayers[l];
        Layer* right = &mLayers[l + 1];
            
        Neuron* leftBeginNeuron = left->beginNeuron;
        Neuron* leftEndNeuron = left->endNeuron;
            
        Neuron* rightBeginNeuron = right->beginNeuron;
        Neuron* rightEndNeuron = right->endNeuron;

        Neuron* leftNeuron = left->beginNeuron;

        for (; leftNeuron != leftEndNeuron; ++leftNeuron)
        {
            Synapse* synapse = leftNeuron->beginSynapse;
            Synapse* endSynapse = leftNeuron->endSynapse;

            Real error = 0.0f;
                
            Neuron* rightNeuron = rightBeginNeuron;

            for (; synapse != endSynapse; ++synapse, ++rightNeuron)
            {
                error += synapse->weight * rightNeuron->delta;
            }

            leftNeuron->delta = error * SigmoidDelta(leftNeuron->value);
        }
    }
}

void ANN::AccumulateGradients()
{
    // E_ij += delta_j * out_i

    for (unsigned int l = 1; l < mLayers.size(); ++l)
    {
        Layer* left = &mLayers[l - 1];
        Layer* right = &mLayers[l];

        Neuron* leftBeginNeuron = left->beginNeuron;
        Neuron* leftEndNeuron = left->endNeuron;
            
        Neuron* rightBeginNeuron = right->beginNeuron;
        Neuron* rightEndNeuron = right->endNeuron;

        Neuron* leftNeuron = leftBeginNeuron;

        for (; leftNeuron != leftEndNeuron; ++leftNeuron)
        {
            Synapse* synapse = leftNeuron->beginSynapse;
            Synapse* endSynapse = leftNeuron->endSynapse;

            Neuron* rightNeuron = rightBeginNeuron;

            Real v = leftNeuron->value;

            for (; synapse != endSynapse; ++synapse, ++rightNeuron)
            {
                assert(rightNeuron != rightEndNeuron
                    && "UpdateWeights(): Invalid memory address.");

                synapse->gradient += v * rightNeuron->delta;
            }
        }
    }
}

void ANN::SetInput(unsigned int index, Real value)
{
    Layer* layer = &mLayers[0];
    (layer->beginNeuron + index)->value = value;
}

Real ANN::GetInput(unsigned int index) const
{
    const Layer* layer = &mLayers[0];
    return (layer->beginNeuron + index)->value;
}

unsigned int ANN::GetInputCount() const
{
    const Layer* layer = &mLayers[0];
    return layer->endNeuron - layer->beginNeuron - 1;
}

void ANN::SetOutput(unsigned int index, Real value)
{
    Layer* layer = &mLayers[mLayers.size() - 1];
    (layer->beginNeuron + index)->value = value;
}

Real ANN::GetOutput(unsigned int index) const
{
    const Layer* layer = &mLayers[mLayers.size() - 1];
    return (layer->beginNeuron + index)->value;
}

unsigned int ANN::GetOutputCount() const
{
    const Layer* layer = &mLayers[mLayers.size() - 1];
    return layer->endNeuron - layer->beginNeuron;
}

void ANN::Run()
{
    // Set all neuron values to zero except input and bias neuron values.
    // There is propably a bit better way to do this..
    for (unsigned int l = 1; l < mLayers.size(); ++l)
    {
        Layer* layer = &mLayers[l];

        Neuron* neuron = layer->beginNeuron;

        Neuron* endNeuron;

        if (l == mLayers.size() - 1)
        {
            endNeuron = layer->endNeuron;
        }

        else
        {
            endNeuron = layer->endNeuron - 1;
        }

        for (; neuron != endNeuron; ++neuron)
        {
            neuron->value = 0.0f;
        }
    }
        
    // Set bias values to 1.
    for (unsigned int l = 0; l < mLayers.size() - 1; ++l)
    {
        Layer* layer = &mLayers[l];

        // Bias.
        (layer->endNeuron - 1)->value = 1.0f;
    }
        
    // Feed forward (go layer by layer).
    for (unsigned int l = 1; l < mLayers.size(); ++l)
    {
        Layer* left = &mLayers[l - 1];
        Layer* right = &mLayers[l];
            
        Neuron* leftBeginNeuron = left->beginNeuron;
        Neuron* leftEndNeuron = left->endNeuron;

        Neuron* rightBeginNeuron = right->beginNeuron;
        Neuron* rightEndNeuron = right->endNeuron;
            
        Neuron* leftNeuron = leftBeginNeuron;

        // Accumulate values.
        for (; leftNeuron != leftEndNeuron; ++leftNeuron)
        {
            Synapse* synapse = leftNeuron->beginSynapse;
            Synapse* endSynapse = leftNeuron->endSynapse;

            Neuron* rightNeuron = rightBeginNeuron;
                
            Real v = leftNeuron->value;
                
            for (; synapse != endSynapse; ++synapse, ++rightNeuron)
            {
                assert(rightNeuron != rightEndNeuron
                    && "Run(): Invalid memory address.");

                rightNeuron->value += v * synapse->weight;
            }
        }
            
        Neuron* rightNeuron = rightBeginNeuron;
            
        // Apply activation.
        for (; rightNeuron != rightEndNeuron; ++rightNeuron)
        {
            rightNeuron->value = Sigmoid(rightNeuron->value);
        }
    }
}
