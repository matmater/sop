#ifndef _ANN_H_
#define _ANN_H_

#include "Common.h"

#include "ANNTrainData.h"

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
	ANN();

	virtual ~ANN();

	void Init();
	
	void SetLearningRate(Real rate);

	Real GetLearningRate() const;

	void SetErrorThreshold(Real threshold);

	Real GetErrorThreshold() const;

	void SetMaxEpochs(unsigned int epochs);

	unsigned int GetMaxEpochs() const;

	void SetTrainingAlgorithm(TrainingAlgorithm algorithm);

	TrainingAlgorithm GetTrainingAlgorithm() const;
	
	void Build();

	void PrintStructure();

	void AddLayer(unsigned int neuronCount);

	void Train(const ANNTrainData& data);

	void PrintProgress();
	
	void CalculateRMSE(const ANNTrainData& data);

	void UpdateWeightsBpropBatch();

	void UpdateWeightsBprop();

	void UpdateWeightsRprop();

	void CalculateOutputDeltas(const ANNTrainData& data, unsigned int sample);
	
	void CalculateHiddenDeltas();

	void AccumulateGradients();

	void SetInput(unsigned int index, Real value);

	Real GetInput(unsigned int index) const;

	unsigned int GetInputCount() const;

	void SetOutput(unsigned int index, Real value);

	Real GetOutput(unsigned int index) const;

	unsigned int GetOutputCount() const;

	void Run();

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