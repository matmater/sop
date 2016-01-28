#include "ANNRecognizer.h"
#include "Utilities.h"

ANNRecognizer::ANNRecognizer()
{

}

ANNRecognizer::~ANNRecognizer()
{

}

void ANNRecognizer::Clear()
{

}

void ANNRecognizer::Train(const SpeechData& data)
{
	if (!data.IsConsistent())
	{
		std::cout << "Inconsistent training data." << std::endl;

		return;
	}

	std::cout << "Initializing run..." << std::endl;

	mNeuronMapping.clear();

	unsigned int sampleCount = data.GetTotalSampleCount();
	unsigned int inputCount = data.GetDimensionCount();
	unsigned int outputCount = data.GetSpeakerCount();

	mTrainData.Init(sampleCount, inputCount, outputCount);

	std::cout << "Registering samples..." << std::endl;

	unsigned int speakerIndex = 0;
	unsigned int sampleIndex = 0;

	for (const auto& entry : data.GetSamples())
	{
		std::cout << "Registering " << entry.first << ", Id: " << speakerIndex << std::endl;

		mNeuronMapping[speakerIndex] = entry.first;

		for (const auto& sample : entry.second)
		{
			for (unsigned int i = 0; i < inputCount; i++)
			{
				mTrainData.SetInput(sampleIndex, i, sample[i]);
			}

			for (unsigned int o = 0; o < outputCount; o++)
			{
				mTrainData.SetOutput(sampleIndex, o, -1.0f);
			}

			mTrainData.SetOutput(sampleIndex, speakerIndex, 1.0f);

			++sampleIndex;
		}

		++speakerIndex;
	}

	std::cout << sampleIndex << " Samples registered for " << speakerIndex << " different speakers." << std::endl;

	std::cout << "Initializing Neural Network..." << std::endl;

	mNetwork.Init();

	mNetwork.SetErrorThreshold(0.001f);
	mNetwork.SetMaxEpochs(50);
	mNetwork.SetLearningRate(0.001f);
	mNetwork.SetTrainingAlgorithm(ANN::TrainingAlgorithm::RPROP);

	std::cout << "Adding layers..." << std::endl;

	// Input layer.
	mNetwork.AddLayer(inputCount);

	// Hidden layers.
	mNetwork.AddLayer(100);

	// Output layer.
	mNetwork.AddLayer(outputCount);

	std::cout << "Building..." << std::endl;

	// Finally build the structure.
	mNetwork.Build();

	std::cout << "Initializing Neural Network DONE." << std::endl;

	std::cout << "Training..." << std::endl;

	mNetwork.Train(mTrainData);
}

void ANNRecognizer::SaveTrainedData(const std::string& path)
{

}

void ANNRecognizer::LoadTrainedData(const std::string& path)
{

}

void ANNRecognizer::Test(const SpeechData& data, std::map<std::string, RecognitionResult>& results)
{
	if (!data.IsConsistent())
	{
		std::cout << "Inconsistent testing data." << std::endl;

		return;
	}

	if (data.GetDimensionCount() != mNetwork.GetInputCount())
	{
		std::cout << "Incompatible testing data." << std::endl;

		return;
	}

	unsigned int outputCount = mNetwork.GetOutputCount();

	mResult.resize(outputCount);

	for (const auto& entry : data.GetSamples())
	{
		std::vector<std::vector<Real>> r;
		bool knownSpeaker = false;

		r.resize(outputCount);

		for (const auto& sample : entry.second)
		{
			unsigned int totalVectors = 0;

			for (unsigned int i = 0; i < sample.GetSize(); i++)
			{
				mNetwork.SetInput(i, sample[i]);
			}

			mNetwork.Run();

			for (unsigned int o = 0; o < outputCount; o++)
			{
				r[o].push_back(mNetwork.GetOutput(o));
			}
		}

		for (unsigned int o = 0; o < outputCount; o++)
		{
			std::sort(r[o].begin(), r[o].end());

			mResult[o] = r[o][r[o].size() / 2];
		}

		unsigned int id = 0;

		unsigned int bestId = -1;
		Real bestValue = std::numeric_limits<Real>::min();

		for (Real res : mResult)
		{
			if (Utilities::IsSameSpeaker(entry.first, mNeuronMapping[id]))
			{
				knownSpeaker = true;
				std::cout << "THIS IS A KNOWN SPEAKER" << std::endl;				
			}
			std::cout << entry.first << "-" << mNeuronMapping[id] << ":" << res << std::endl;

			if (res > bestValue)
			{
				bestValue = res;
				bestId = id;
			}

			++id;
		}

		if (bestId != -1)
		{
			if (bestValue >= 0.75f)
			{
				results[entry.first] = RecognitionResult(knownSpeaker,  mNeuronMapping[bestId]);
			}

			else
			{
				results[entry.first] = RecognitionResult(knownSpeaker);
			}
		}

		else
		{
			results[entry.first] = RecognitionResult(knownSpeaker);
		}
	}

	std::cout << "Testing DONE." << std::endl;
}
