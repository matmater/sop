#include "SpeechData.h"

SpeechData::SpeechData()
    : mConsistent(true),
    mDimensionCount(0)
{

}

SpeechData::~SpeechData()
{

}

void SpeechData::Load(const std::string& path)
{
    Clear();

    std::ifstream file(path);

    std::string line;

    unsigned int totalLines = 0;

    while (std::getline(file, line))
    {
        ++totalLines;
    }

    file.clear();
    file.seekg(0);

    unsigned int lineCounter = 0;

    while(std::getline(file, line))
    {
        std::stringstream ssFeatures(line);

        std::string label;

        if (ssFeatures >> label)
        {
            std::cout << "Loading samples: " << label
                << " (" << 100 * lineCounter / totalLines << "%)" << std::endl;

            std::string features;

            auto& userSamples = mSamples[label];

            while (std::getline(ssFeatures, features, ','))
            {
                userSamples.emplace_back();

                std::stringstream ssFeature(features);
                std::string feature;

                while (std::getline(ssFeature, feature, ' '))
                {
                    if (feature.size() > 0 && feature[0] != ' ')
                    {
                        userSamples.back().Push(ConvertString<Real>(feature));
                    }
                }

                if (userSamples.back().GetSize() == 0)
                {
                    userSamples.pop_back();
                }
            }

            if (userSamples.size() == 0)
            {
                mSamples.erase(label);

                std::cout << "Error: missing sample data." << std::endl;
            }
        }

        ++lineCounter;
    }

    Validate();
}

void SpeechData::Validate()
{
    mTotalSampleCount = 0;
    mConsistent = false;

    std::cout << "Validating..." << std::endl;

    std::cout << "Calculating inputs..." << std::endl;

    unsigned int featureCount = 0;

    if (mSamples.size() > 0
        && (*mSamples.begin()).second.size() > 0) // Samples
    {
        // First feature vector of first sample.
        featureCount = (*mSamples.begin()).second.front().GetSize();
    }

    std::cout << "Checking training sample data dimensions and calculating sample training count..." << std::endl;

    for (const auto& entry : mSamples)
    {
        std::cout << "Checking samples of " << entry.first << "..." << std::endl;

        for (const auto& sample : entry.second)
        {
            if (sample.GetSize() != featureCount)
            {
                std::cout << featureCount << std::endl;
                std::cout << sample.GetSize() << std::endl;
                std::cout << "Error: feature count mismatch." << std::endl;

                mTotalSampleCount = 0;

                return;
            }

            ++mTotalSampleCount;
        }
    }

    std::cout << "Data CONSISTENT." << std::endl;

    mDimensionCount = featureCount;
    mConsistent = true;
}

void SpeechData::Clear()
{
    mSamples.clear();
    mConsistent = true;
    mTotalSampleCount = 0;
    mDimensionCount = 0;
}

bool SpeechData::IsConsistent() const
{
    return mConsistent;
}

bool SpeechData::IsCompatible(const SpeechData& other)
{
    return
        (mDimensionCount == other.mDimensionCount)
        && mConsistent
        && other.mConsistent;
}

unsigned int SpeechData::GetDimensionCount() const
{
    return mDimensionCount;
}

void SpeechData::Normalize()
{
    for (auto& seq : mSamples)
    {
        for (auto& s : seq.second)
        {
            s.Multiply(1.0f / 26.0f);
            s.Add(1.0f);
        }
    }
}

unsigned int SpeechData::GetSpeakerCount() const
{
    return mSamples.size();
}

unsigned int SpeechData::GetTotalSampleCount() const
{
    return mTotalSampleCount;
}

const std::map<std::string, std::vector<DynamicVector<Real> > >& SpeechData::GetSamples() const
{
    return mSamples;
}
