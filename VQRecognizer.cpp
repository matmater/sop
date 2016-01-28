#include "VQRecognizer.h"
#include "Utilities.h"

VQRecognizer::VQRecognizer()
	: mBackgroundModelEnabled(false)
{

}

VQRecognizer::~VQRecognizer()
{

}

void VQRecognizer::SetBackgroundModelEnabled(bool enabled)
{
	mBackgroundModelEnabled = enabled;
}

bool VQRecognizer::IsBackgroundModelEnabled() const
{
	return mBackgroundModelEnabled;
}

unsigned int VQRecognizer::GetDimensionCount()
{
	if (mCodebooks.size() == 0)
		return 0;

	if (mCodebooks.begin()->second.clusterCentroids.size() == 0)
		return 0;

	return mCodebooks.begin()->second.clusterCentroids.front().GetSize();
}

Real VQRecognizer::Distortion(const Codebook& codebook, const std::vector< DynamicVector<Real> >& samples)
{
	// INVERSE???

	if (mClusterSamples.size() < samples.size())
	{
		mClusterSamples.resize(samples.size());
	}

	auto& centroids = codebook.clusterCentroids;
	auto& sizes = codebook.clusterSizes;
	auto& weights = codebook.clusterWeights;

	Real dist = 0.0f;

	for (unsigned int s = 0; s < samples.size(); ++s)
	{
		auto& sample = samples[s];

		Real minDist = std::numeric_limits<Real>::max();
		unsigned int minC = -1;

		for (unsigned int c = 0; c < centroids.size(); ++c)
		{
			if (sizes[c] == 0)
			{
				continue;
			}

			dist = sample.Distance(centroids[c]);

			if (dist < minDist)
			{
				minDist = dist;
				minC = c;
			}
		}

		mClusterSamples[s] = minC;
	}

	Real distortion = 0.0f;

	for (unsigned int s = 0; s < samples.size(); ++s)
	{
		distortion += weights[mClusterSamples[s]]
			/ samples[s].Distance(centroids[mClusterSamples[s]]);
	}

	return distortion;
}

void VQRecognizer::Clear()
{

}

void VQRecognizer::Test(const SpeechData& data, std::map<std::string, RecognitionResult>& results)
{
	if (!data.IsConsistent())
	{
		std::cout << "Inconsistent testing data." << std::endl;

		return;
	}

	if (GetDimensionCount() != data.GetDimensionCount())
	{
		std::cout << "Incompatible testing data dimensions." << std::endl;

		return;
	}

	Codebook* ubm = nullptr;

	auto it = mCodebooks.find(".ubm");

	if (mBackgroundModelEnabled && it != mCodebooks.end())
	{
		ubm = &it->second;
	}

	// Clear old results (if any).
	results.clear();

	for (auto& entry : data.GetSamples())
	{
		std::string bestMatch = "";

		Real minMatchDist = std::numeric_limits<Real>::min();
		Real divDistortion;
		bool knownSpeaker = false;
		Codebook* bestCodebook = nullptr;

		for (auto& codebook : mCodebooks)
		{
			if (codebook.first == ".ubm") continue;
			if (Utilities::IsSameSpeaker(entry.first, codebook.first))
			{
				knownSpeaker = true;				
			}
			Real distortion = Distortion(codebook.second, entry.second);

			DEBUG_CONTENT(
				Real distortionUBM = -1.0f;

				if (ubm != nullptr)
				{
					distortionUBM = Distortion(*ubm, entry.second);
				}
			)

			divDistortion = distortion / static_cast<Real>(entry.second.size());

			DEBUG_TRACE(entry.first << "-" << codebook.first << ":"
				<< divDistortion
				<< " ubm:"
				<< std::log(distortion / distortionUBM));

			if (divDistortion > minMatchDist)
			{
				minMatchDist = divDistortion;
				bestCodebook = &codebook.second;
				bestMatch = codebook.first;
			}
		}

		DEBUG_TRACE("The most probable speaker is " << bestMatch << ".");

		if (bestCodebook != nullptr)
		{
			// Checking for nullptr ensures that UBM is enabled
			// and the model actually exists.
			if (ubm != nullptr)
			{
				// Divide minMatchDist by UBM distortion.
				Real ratio = std::log(Distortion(*bestCodebook, entry.second) / Distortion(*ubm, entry.second));

				// UBM is not too close to bestMatch.
				if (ratio > 0.3f && minMatchDist >= 0.135f) // UBM Threshold
				{
					results[entry.first] = RecognitionResult(knownSpeaker, bestMatch);
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
				if (minMatchDist >= 0.135f)
				{
					results[entry.first] = RecognitionResult(knownSpeaker, bestMatch);
				}

				else
				{
					results[entry.first] = RecognitionResult(knownSpeaker);
				}
			}
		}

		// Should happen only if there are no codebooks trained. Still unrecognized?
		else
		{
			results[entry.first] = RecognitionResult(knownSpeaker);
		}
	}
}

void VQRecognizer::Train(const SpeechData& data)
{
	if (!data.IsConsistent())
	{
		std::cout << "Inconsistent training data." << std::endl;

		return;
	}

	unsigned int progress = 0;

	for (const auto& sequence : data.GetSamples())
	{
		std::cout << "Training codebook: " << sequence.first
			<< " (" << 100 * progress / data.GetSamples().size() << "%)" << std::endl;

		Codebook& codebook = mCodebooks[sequence.first];

		Train(codebook, sequence.second);

		++progress;
	}
}

void VQRecognizer::Train(Codebook& codebook, const std::vector< DynamicVector<Real> >& samples)
{
	LBG lbg;

	std::vector<unsigned int> indices;
	indices.resize(samples.size());

	codebook.clusterCentroids.resize(128);
	codebook.clusterSizes.resize(128);
	codebook.clusterWeights.resize(128);

	lbg.Cluster(samples, indices, codebook.clusterCentroids, codebook.clusterSizes);

	for (auto& a : mCodebooks)
	{
		for (unsigned int i = 0; i < a.second.clusterCentroids.size(); i++)
		{
			if (a.second.clusterSizes[i] == 0)
			{
				continue;
			}

			Real sum = 0.0f;

			for (auto& b : mCodebooks)
			{
				if (a.first == b.first)
				{
					continue;
				}

				Real dmin = std::numeric_limits<Real>::max();

				for (unsigned int j = 0; j < b.second.clusterCentroids.size(); j++)
				{
					if (b.second.clusterSizes[j] == 0)
					{
						continue;
					}

					Real dist = b.second.clusterCentroids[j].Distance(a.second.clusterCentroids[i]);

					if (dist < dmin)
					{
						dmin = dist;
					}
				}

				sum += 1.0f / dmin;
			}

			a.second.clusterWeights[i] = 1.0f / sum;
		}
	}
}

void VQRecognizer::SaveTrainedData(const std::string& path)
{
	std::ofstream outFile;

	outFile.open(path);

	for (const auto& entry : mCodebooks)
	{
		const auto& centroids = entry.second.clusterCentroids;
		const auto& sizes = entry.second.clusterSizes;
		const auto& weights = entry.second.clusterWeights;

		outFile << entry.first << " ";

		for (unsigned int c = 0; c < centroids.size(); c++)
		{
			outFile << weights[c] << " ";

			for (unsigned int i = 0; i < centroids[c].GetSize(); i++)
			{
				outFile << centroids[c][i];

				if (i + 1 != centroids[c].GetSize())
				{
					outFile << " ";
				}
			}

			if (c + 1 != centroids.size())
			{
				outFile << ",";
			}
		}

		outFile << std::endl;
	}
}

void VQRecognizer::LoadTrainedData(const std::string& path)
{
	mCodebooks.clear();

	std::ifstream inFile;

	inFile.open(path);

	std::string line;

	while(std::getline(inFile, line))
	{
		std::stringstream ssFeatures(line);

		std::string label;

		if (ssFeatures >> label)
		{
			std::cout << "Loading model: " << label << std::endl;

			std::string features;

			auto& codebook = mCodebooks[label];

			while (std::getline(ssFeatures, features, ','))
			{
				codebook.clusterCentroids.emplace_back();
				codebook.clusterSizes.emplace_back();
				codebook.clusterWeights.emplace_back();

				std::stringstream ssFeature(features);
				std::string feature;

				if (ssFeature >> feature)
				{
					std::cout << "weight:" << ConvertString<Real>(feature) << std::endl;
					codebook.clusterWeights.back() = ConvertString<Real>(feature);
				}

				while (std::getline(ssFeature, feature, ' '))
				{
					if (feature.size() > 0 && feature[0] != ' ')
					{
						codebook.clusterCentroids.back().Push(ConvertString<Real>(feature));
					}
				}

				codebook.clusterSizes.back() = 1;

				if (codebook.clusterCentroids.back().GetSize() == 0)
				{
					codebook.clusterCentroids.pop_back();
					codebook.clusterSizes.pop_back();
					codebook.clusterWeights.pop_back();
				}
			}

			if (codebook.clusterCentroids.size() == 0)
			{
				mCodebooks.erase(label);

				std::cout << "Error: missing sample data." << std::endl;
			}

			std::cout
				<< "current codebook: " << label
				<< "added codebook count: " << mCodebooks.size()
				<< ", codebook centroids: " << codebook.clusterCentroids.size()
				<< std::endl;
		}
	}
}
