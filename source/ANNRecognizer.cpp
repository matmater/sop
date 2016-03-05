/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
 */

#include "ANNRecognizer.h"

ANNRecognizer::ANNRecognizer()
{

}

ANNRecognizer::~ANNRecognizer()
{

}

void ANNRecognizer::Clear()
{
    mSpeakerData = nullptr;
}

void ANNRecognizer::SetSpeakerData(std::shared_ptr<SpeechData> data)
{
    mSpeakerData = data;
}

std::shared_ptr<SpeechData> ANNRecognizer::GetSpeakerData()
{
    return mSpeakerData;
}

void ANNRecognizer::Train()
{
    if (mSpeakerData == nullptr)
    {
        std::cout << "Speaker data not found." << std::endl;

        return;
    }

    if (!mSpeakerData->IsConsistent())
    {
        std::cout << "Inconsistent training data." << std::endl;

        return;
    }

    std::cout << "Initializing run..." << std::endl;

    mNeuronMapping.clear();

    unsigned int sampleCount = mSpeakerData->GetTotalSampleCount();
    unsigned int inputCount = mSpeakerData->GetDimensionCount();
    unsigned int outputCount = mSpeakerData->GetSpeakerCount();

    mTrainData.Init(sampleCount, inputCount, outputCount);

    std::cout << "Registering samples..." << std::endl;

    unsigned int speakerIndex = 0;
    unsigned int sampleIndex = 0;

    for (const auto& entry : mSpeakerData->GetSamples())
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

bool ANNRecognizer::IsRecognized(const SpeakerKey& speaker, const std::vector< DynamicVector<Real> >& samples)
{
    //if (samples.size() == 0 || samples.front().GetSize() != mNetwork.GetInputCount())
    //{
    //    std::cout << "Incompatible testing data." << std::endl;

    //    return;
    //}

    //bool found = false;

    //for (auto& spk : mNeuronMapping)
    //{
    //    if (spk.second == speaker)
    //    {
    //        found = true;
    //    }
    //}

    //if (!found)
    //{
    //    std::cout << "Unknown speaker." << std::endl;

    //    return;
    //}

    //unsigned int outputCount = mNetwork.GetOutputCount();

    //std::vector<Real> result(outputCount, 0.0f);

    //for (const auto& entry : samples)
    //{
    //    for (unsigned int i = 0; i < entry.GetSize(); i++)
    //    {
    //        mNetwork.SetInput(i, entry[i]);
    //    }

    //    mNetwork.Run();

    //    for (unsigned int o = 0; o < outputCount; o++)
    //    {
    //        result[o] += mNetwork.GetOutput(o) / static_cast<Real>(samples.size());
    //    }
    //}

    //unsigned int bestIndex = -1;
    //Real bestScore = std::numeric_limits<Real>::min();
    //
    //for (unsigned int o = 0; o < outputCount; o++)
    //{
    //    if (result[o] > bestScore)
    //    {
    //        bestScore = result[o];
    //        bestIndex = o;
    //    }
    //}

    //return speaker == mNeuronMapping[bestIndex];

    return false;
}

std::vector<Real> ANNRecognizer::Verify(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data)
{
    //if (data->GetDimensionCount() != mNetwork.GetInputCount())
    //{
    //    std::cout << "Incompatible testing data." << std::endl;

    //    return;
    //}
    //
    //unsigned int outputCount = mNetwork.GetOutputCount();

    //bool found = false;

    //unsigned int speakerIndex = 0;

    //for (auto& spk : mNeuronMapping)
    //{
    //    if (spk.second == speaker)
    //    {
    //        found = true;
    //        speakerIndex = spk.first;
    //        break;
    //    }
    //}

    //if (!found)
    //{
    //    std::cout << "Unknown speaker." << std::endl;

    //    return;
    //}

    //std::vector<Real> results;
    //
    //for (auto& entry : data->GetSamples())
    //{
    //    std::vector<Real> result(outputCount, 0.0f);

    //    for (const auto& samples : entry.second)
    //    {
    //        for (unsigned int i = 0; i < samples.GetSize(); i++)
    //        {
    //            mNetwork.SetInput(i, samples[i]);
    //        }

    //        mNetwork.Run();

    //        for (unsigned int o = 0; o < outputCount; o++)
    //        {
    //            result[o] += mNetwork.GetOutput(o) / static_cast<Real>(entry.second.size());
    //        }
    //    }

    //    results.push_back(result[speakerIndex]);
    //}

    //return results;

    return std::vector<Real>();
}