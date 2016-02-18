#include "SpeechData.h"

SpeechData::SpeechData()
    : mNormalizationType(FeatureNormalizationType::CEPSTRAL_MEAN),
    mConsistent(true),
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
        SpeakerKey key;

        if (ssFeatures >> label)
        {
            std::cout << "Loading samples: " << label
                << " (" << 100 * lineCounter / totalLines << "%)" << std::endl;

            std::string features;

            key = SpeakerKey(label);

            auto& userSamples = mSamples[SpeakerKey(label)];

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
                mSamples.erase(key);

                std::cout << "Error: missing sample data." << std::endl;
            }
        }

        ++lineCounter;
    }

    Validate();
}

void SpeechData::Load(const std::string& path, unsigned int sl, unsigned int gl, bool train, const std::string& alias)
{
    // \todo CHECK BUGS!

    //Clear();

    std::ifstream file(path);

    std::string line;

    unsigned int totalLines = sl+gl-1;
    /*
    while (std::getline(file, line))
    {
        ++totalLines;
    }
    
    totalLines = std::min(totalLines, sl+gl-1);
    std::cout << "Lines: " << totalLines << std::endl;


    file.clear();
    file.seekg(0);
    */

    unsigned int lineCounter = 1;
    
    while(std::getline(file, line))
    {
        if (lineCounter >= sl)
        {
            SpeechDataBuffer buffer(const_cast<char*>(line.c_str()), line.size());
            std::istream ssFeatures(&buffer);

            std::string label;
            SpeakerKey key;

            if (ssFeatures >> label)
            {
                if (alias.size() >= 3)
                {
                    label[0] = alias[0];
                    label[1] = alias[1];
                    label[2] = alias[2];
                }

                if (train)
                {
                    label.erase(3);
                }
                
                std::cout << "Loading samples: " << label
                    << " (" << 100 * (lineCounter - sl + 1) / (totalLines - sl + 1) << "%)" << std::endl;

                std::string features;

                key = SpeakerKey(label);

                auto& userSamples = mSamples[key];

                while (std::getline(ssFeatures, features, ','))
                {
                    userSamples.emplace_back();

                    SpeechDataBuffer buffer2(const_cast<char*>(features.c_str()), features.size());
                    std::istream ssFeature(&buffer2);
                    std::string feature;

                    while (std::getline(ssFeature, feature, ' '))
                    {
                        if (feature.size() > 0 && feature[0] != ' ')
                        {
                            // \todo CHECK
                            userSamples.back().Push(std::stof(feature));
                        }
                    }

                    if (userSamples.back().GetSize() == 0)
                    {
                        userSamples.pop_back();
                    }
                }

                if (userSamples.size() == 0)
                {
                    mSamples.erase(key);

                    std::cout << "Error: missing sample data." << std::endl;
                }
            }
        }

        if (lineCounter == totalLines)
        {
            break;
        }

        ++lineCounter;
    }

    //Validate();
}

void SpeechData::Validate()
{
    mTotalSampleCount = 0;
    mConsistent = false;

    std::cout << "Validating speech data..." << std::endl;

    unsigned int featureCount = 0;

    if (mSamples.size() > 0
        && (*mSamples.begin()).second.size() > 0) // Samples
    {
        // First feature vector of first sample.
        featureCount = (*mSamples.begin()).second.front().GetSize();
    }

    for (const auto& entry : mSamples)
    {
        for (const auto& sample : entry.second)
        {
            if (sample.GetSize() != featureCount)
            {
                std::cout << featureCount << std::endl;
                std::cout << sample.GetSize() << std::endl;
                std::cout << "Speech data not valid: feature count mismatch." << std::endl;

                mTotalSampleCount = 0;

                return;
            }

            ++mTotalSampleCount;
        }
    }

    std::cout << "Speech data valid." << std::endl;

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

void SpeechData::SetNormalizationType(FeatureNormalizationType normalizationType)
{
    mNormalizationType = normalizationType;
}

FeatureNormalizationType SpeechData::GetNormalizationType() const
{
    return mNormalizationType;
}

void SpeechData::Normalize()
{
    //for (auto& seq : mSamples)
    //{
    //    for (auto& s : seq.second)
    //    {
    //        s.Multiply(1.0f / 39.0f);
    //    }
    //}

    switch (mNormalizationType)
    {
    case FeatureNormalizationType::NONE:
        break;

    case FeatureNormalizationType::CEPSTRAL_MEAN:
        {
            DynamicVector<Real> means;
            DynamicVector<Real> deviations;

            means.Resize(GetDimensionCount());
            deviations.Resize(GetDimensionCount());

            // Normalize entry by entry.
            for (auto& sample : mSamples)
            {
                // Normalize dimensions separately.
                for (unsigned int d = 0; d < GetDimensionCount(); d++)
                {
                    std::vector<Real> values;

                    for (const auto& value : sample.second)
                    {
                        values.push_back(value[d]);
                    }

                    // Get mean & deviation over all samples.
                    Real mean = Mean(values);
                    Real deviation = Deviation(values, mean);

                    means[d] = mean;
                    deviations[d] = deviation;
                }

                for (auto& value : sample.second)
                {
                    value.Subtract(means);
                    value.Divide(deviations);
                }
            }
        }

        break;

    default:
        std::cout << "Unknown feature normalization type." << std::endl;
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

const std::map<SpeakerKey, std::vector<DynamicVector<Real> > >& SpeechData::GetSamples() const
{
    return mSamples;
}

void LoadTextSamples(const std::string& folder, const std::shared_ptr<SpeechData>& data, unsigned int sf, unsigned int gf, unsigned int sl, unsigned int gl, bool train)
{
    // \todo CHECK BUGS!

    data->Clear();
    for (unsigned int i = 0; i < gf; i++)
    {
#ifdef INDEXFIX
        static std::vector<std::string> speakerFiles;
        static bool speakerFilesScanned = false;

        if (!speakerFilesScanned)
        {
            for (unsigned int i = 225; i <= 376; ++i)
            {
                std::string fname = folder + "/samples_" + toString(i) + ".txt";

                if (FileExists(fname))
                {
                    speakerFiles.push_back(fname);
                }
            }

            speakerFilesScanned = true;
        }

        std::cout << speakerFiles[i+sf-225] << std::endl;
        data->Load(speakerFiles[i+sf-225], sl, gl, train, toString(sf+i));
#else
        std::string file = folder + "/samples_" + toString(sf+i) + ".txt";
        std::cout << file << std::endl;
        data->Load(file, sl, gl, train);
#endif
    }
    data->Validate();
    data->Normalize();
}