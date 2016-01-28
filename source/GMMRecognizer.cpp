#include "GMMRecognizer.h"
#include "Utilities.h"

GMMRecognizer::GMMRecognizer()
: mBackgroundModelEnabled(false), mClusterCount(128)
{
    mValid = false;
}

GMMRecognizer::~GMMRecognizer()
{

}

void GMMRecognizer::SetBackgroundModelEnabled(bool enabled)
{
    mBackgroundModelEnabled = enabled;
}

bool GMMRecognizer::IsBackgroundModelEnabled() const
{
    return mBackgroundModelEnabled;
}

void GMMRecognizer::SetClusterCount(unsigned int clusterCount)
{
    mClusterCount = clusterCount;
}
    
unsigned int GMMRecognizer::GetClusterCount() const
{
    return mClusterCount;
}

void GMMRecognizer::Clear()
{

}

void GMMRecognizer::Train(const SpeechData& data)
{
    if (!data.IsConsistent())
    {
        std::cout << "Inconsistent training data." << std::endl;

        return;
    }

    unsigned int progress = 0;

    for (auto& sequence : data.GetSamples())
    {
        std::cout << "Training model: " << sequence.first
            << " (" << 100 * progress / data.GetSamples().size() << "%)" << std::endl;

        GMM& model = mModels[sequence.first];
        model.SetClusterCount(mClusterCount);

        model.Train(sequence.second);

        ++progress;
    }
}

void GMMRecognizer::Test(const SpeechData& data, std::map<std::string, RecognitionResult>& results)
{
    if (!data.IsConsistent())
    {
        std::cout << "Inconsistent testing data." << std::endl;

        return;
    }

    results.clear();

    GMM* ubm = nullptr;

    auto it = mModels.find(".ubm");

    if (mBackgroundModelEnabled && it != mModels.end())
    {
        ubm = &it->second;
    }

    for (auto& entry : data.GetSamples())
    {
        GMM* bestModel = nullptr;

        Real maxValue = std::numeric_limits<Real>::min();

        std::string bestModelName = "";
        bool knownSpeaker = false;

        for (auto& model : mModels)
        {
            if (model.first == ".ubm") continue;
            if (Utilities::IsSameSpeaker(entry.first, model.first))
            {
                knownSpeaker = true;
                std::cout << "THIS IS A KNOWN SPEAKER" << std::endl;
            }
            if (ubm != nullptr)
            {
                DEBUG_TRACE(entry.first << "-" << model.first << ":" << model.second.GetValue(entry.second)
                    << " ubm:"
                    << std::log(model.second.GetValue(entry.second) / ubm->GetValue(entry.second))
                    << ","
                    << model.second.GetValue(entry.second) / ubm->GetValue(entry.second));
            }

            else
            {
                DEBUG_TRACE(entry.first << "-" << model.first << ":" << model.second.GetValue(entry.second));
            }

            Real value = model.second.GetValue(entry.second);

            if (value > maxValue)
            {
                bestModel = &model.second;
                bestModelName = model.first;
                maxValue = value;
            }
        }

        if (bestModel != nullptr)
        {
            // Checking for nullptr ensures that UBM is enabled
            // and the model actually exists.
            if (ubm != nullptr)
            {
                // Divide minMatchDist by UBM distortion.
                Real ratio = std::log(bestModel->GetValue(entry.second) / ubm->GetValue(entry.second));

                // UBM is not too close to bestMatch.
                if (ratio > 0.3f && maxValue >= 10e10f) // UBM Threshold
                {
                    results[entry.first] = RecognitionResult(knownSpeaker, bestModelName);
                }

                // Too close to UBM.
                else
                {
                    results[entry.first] = RecognitionResult(knownSpeaker);
                }
            }

            // UBM is not enabled. Select using threshold.
            else
            {
                if (maxValue >= 10e10f)
                {
                    results[entry.first] = RecognitionResult(knownSpeaker, bestModelName);
                }

                else
                {
                    results[entry.first] = RecognitionResult(knownSpeaker);
                }
            }
        }

        else
        {
            results[entry.first] = RecognitionResult(knownSpeaker);
        }
    }
}

void GMMRecognizer::SaveTrainedData(const std::string& path)
{
    //std::ofstream outFile;

    //outFile.open(path);

    //for (const auto& model : mModels)
    //{
    //	const auto& clusters = model.second.GetClusters();

    //	outFile << model.first << " ";
    //
    //	unsigned int mc = 0;
    //	unsigned int vc = 0;

    //	for (auto it = clusters.begin(); it != clusters.end(); ++it)
    //	{
    //		std::cout << "mixingCoefficient: " << it->mixingCoefficient << " ";
    //
    //		for (auto fit = it->means.begin(); fit != it->means.end(); ++fit)
    //		{
    //			outFile << *fit;
    //			++mc;
    //
    //			if (fit + 1 != it->variances.end())
    //			{
    //				outFile << " ";
    //			}
    //		}
    //
    //		outFile << "|";

    //		for (auto fit = it->variances.begin(); fit != it->variances.end(); ++fit)
    //		{
    //			outFile << *fit;
    //			++vc;

    //			if (fit + 1 != it->variances.end())
    //			{
    //				outFile << " ";
    //			}
    //		}

    //		if (it + 1 != clusters.end())
    //		{
    //			outFile << ",";
    //		}
    //
    //		std::cout << "means: " << mc << ", ";
    //		std::cout << "variances: " << vc << std::endl;
    //	}

    //	outFile << std::endl;
    //}
}

void GMMRecognizer::LoadTrainedData(const std::string& path)
{
    //mModels.clear();

    //std::ifstream inFile;

    //inFile.open(path);

    //std::string line;

    //while(std::getline(inFile, line))
    //{
    //	std::stringstream ssFeatures(line);

    //	std::string label;

    //	if (ssFeatures >> label)
    //	{
    //		std::cout << "Loading model: " << label << std::endl;

    //		std::string features;
    //
    //		auto& clusters = mModels[label].GetClusters();

    //		while (std::getline(ssFeatures, features, ','))
    //		{
    //			clusters.emplace_back();
    //
    //			std::stringstream ssFeature(features);
    //			std::string feature;
    //
    //			if (ssFeature >> feature)
    //			{
    //				std::cout << "mixingCoefficient: " << std::stof(feature) << std::endl;
    //				clusters.back().mixingCoefficient = std::stof(feature);
    //			}
    //
    //			unsigned int mc = 0;
    //			unsigned int vc = 0;

    //			while (std::getline(ssFeature, feature, ' '))
    //			{
    //				if (feature[0] == '|')
    //				{
    //					break;
    //				}

    //				if (feature.size() > 0 && feature[0] != ' ')
    //				{
    //					clusters.back().means.push_back(std::stof(feature));
    //					++mc;
    //				}
    //			}

    //			while (std::getline(ssFeature, feature, ' '))
    //			{
    //				if (feature[0] == '|')
    //				{
    //					break;
    //				}

    //				if (feature.size() > 0 && feature[0] != ' ')
    //				{
    //					clusters.back().variances.push_back(std::stof(feature));
    //					++vc;
    //				}
    //			}
    //
    //			std::cout << "means: " << mc << ", ";
    //			std::cout << "variances: " << vc << std::endl;
    //
    //			if (clusters.back().means.size() == 0)
    //			{
    //				clusters.pop_back();
    //			}
    //		}
    //
    //		if (clusters.size() == 0)
    //		{
    //			mModels.erase(label);

    //			std::cout << "Error: missing sample data." << std::endl;
    //		}
    //
    //		std::cout << mModels.size() << clusters.size() << std::endl;
    //	}
    //}
}
