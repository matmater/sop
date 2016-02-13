#include "GMMRecognizer.h"

GMMRecognizer::GMMRecognizer()
{

}

GMMRecognizer::~GMMRecognizer()
{

}

void GMMRecognizer::Train()
{
    ModelRecognizer<GMModel>::Train();
}

void GMMRecognizer::Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results)
{
    ModelRecognizer<GMModel>::Test(data, results);
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
