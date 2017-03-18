/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#include "SpeechData.h"

SpeechData::SpeechData()
    : mNormalizationType(FeatureNormalizationType::CEPSTRAL_MEAN_VARIANCE),
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

void SpeechData::Load(const std::string& path, unsigned int sl, unsigned int gl, unsigned int multiplier, bool train, unsigned int maxFeatures, bool normalize, const std::string& alias)
{
    std::ifstream file(path);

    std::string line;

    unsigned int lineCounter = 1;

    if (train)
    {
        std::cout << "Loading train samples." << std::endl;
    }

    else
    {
        std::cout << "Loading samples, multiplier: " << multiplier
                  << ", total lines: " << multiplier << "*" << gl << "=" << multiplier * gl << std::endl;
    }

    if (!train && multiplier > 1)
    {
        gl = multiplier * gl;
    }

    unsigned int totalLines = sl+gl-1;

    unsigned int lc = 0;

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
                std::string oldLabel = label;

                if (alias.size() >= 3)
                {
                    label[0] = alias[0];
                    label[1] = alias[1];
                    label[2] = alias[2];
                }

                if (train || multiplier > 1)
                {
                    label.erase(3);
                }

                if (!train && multiplier > 1)
                {
                    std::stringstream ss;
                    ss << label << "_" << (lc / multiplier) + 1;
                    label = ss.str();

                    std::cout << "Loading samples from '" << oldLabel << "' to '" << label << "'" << " mul " << multiplier
                        << " (" << 100 * (lineCounter - sl + 1) / (totalLines - sl + 1) << "%)" << std::endl;
                }

                else
                {
                    std::cout << "Loading samples from '" << oldLabel << "' to '" << label << "'"
                        << " (" << 100 * (lineCounter - sl + 1) / (totalLines - sl + 1) << "%)" << std::endl;
                }

                std::string features;

                key = SpeakerKey(label);

                auto& userSamples = mSamples[key];

                unsigned int totalVectors = 0;

                // Feature vectors separated by commas.
                while (std::getline(ssFeatures, features, ','))
                {
                    userSamples.emplace_back();

                    SpeechDataBuffer buffer2(const_cast<char*>(features.c_str()), features.size());
                    std::istream ssFeature(&buffer2);
                    std::string feature;

                    unsigned int featureCount = 0;

                    // Features separated by spaces.
                    while (std::getline(ssFeature, feature, ' '))
                    {
                        if (feature.size() > 0 && feature[0] != ' ')
                        {
                            if (featureCount++ == maxFeatures) break;

                            userSamples.back().Push(std::stof(feature));
                        }
                    }

                    if (userSamples.back().GetSize() > 0)
                    {
                        ++totalVectors;
                    }

                    else
                    {
                        userSamples.pop_back();
                    }
                }

                if (userSamples.size() == 0)
                {
                    mSamples.erase(key);

                    std::cout << "Error: missing sample data." << std::endl;
                }

                else
                {
                    // Per-utterance normalization.
                    if (normalize)
                    {
                        std::cout << "Normalizing utterance of size: " << totalVectors << "." << std::endl;
                        Normalize(userSamples.end() - totalVectors, userSamples.end());
                    }
                }
            }

            ++lc;
        }

        if (lineCounter == totalLines)
        {
            break;
        }

        ++lineCounter;
    }
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

void SpeechData::CMVN(
    std::vector < DynamicVector<Real> >::iterator beginIt,
    std::vector < DynamicVector<Real> >::iterator endIt)
{
    DynamicVector<Real> means;
    DynamicVector<Real> deviations;

    means.Resize(beginIt->GetSize());
    deviations.Resize(beginIt->GetSize());

    // Normalize dimensions separately.
    for (unsigned int d = 0; d < beginIt->GetSize(); d++)
    {
        std::vector<Real> values;

        for (auto it = beginIt; it != endIt; it++)
        {
            values.push_back((*it)[d]);
        }

        // Get mean & deviation over all samples.
        Real mean = Mean(values);
        Real deviation = Deviation(values, mean);

        means[d] = mean;
        deviations[d] = deviation;
    }

    for (auto it = beginIt; it != endIt; it++)
    {
        it->Subtract(means);
        it->Divide(deviations);
    }
}

void SpeechData::Normalize(
    std::vector < DynamicVector<Real> >::iterator beginIt,
    std::vector < DynamicVector<Real> >::iterator endIt)
{
    switch (mNormalizationType)
    {
    case FeatureNormalizationType::NONE:
        break;

    case FeatureNormalizationType::CEPSTRAL_MEAN_VARIANCE:
        CMVN(beginIt, endIt);

        break;

    default:
        std::cout << "Unknown feature normalization type." << std::endl;
    }
}

void SpeechData::Normalize()
{
    // Normalize entry by entry.
    for (auto& sample : mSamples)
    {
        Normalize(sample.second.begin(), sample.second.end());
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

void LoadTextSamples(const std::string& folder, const std::shared_ptr<SpeechData>& data, unsigned int sf, unsigned int gf, unsigned int sl, unsigned int gl, unsigned int multiplier, bool train)
{
    if (folder.size() == 0)
    {
        std::cout << "Could not load samples: Missing folder name." << std::endl;
        return;
    }

    bool cmvn = false;
    unsigned int maxFeatures = 39;

    std::stringstream ss(folder);

    std::string finalFolder;

    if (!std::getline(ss, finalFolder, '_'))
    {
        std::cout << "Invalid folder name." << std::endl;
        return;
    }

    std::string str;

    while (std::getline(ss, str, '_'))
    {
        if (str == "cmvn")
        {
            cmvn = true;
        }

        else if (str.size() > 0 && str[0] == 'f')
        {
            str.erase(0, 1);

            try
            {
                maxFeatures = std::stoi(str);
            }

            catch (...)
            {
                std::cout << "Invalid feature count format." << std::endl;
            }
        }
    }

    data->Clear();
    for (unsigned int i = 0; i < gf; i++)
    {
        std::string file = finalFolder + "/samples_" + toString(sf+i) + ".txt";
        std::cout << file << std::endl;
        data->Load(file, sl, gl, multiplier, train, maxFeatures, false, GetSpeakerString(sf + i, folder));
    }
    data->Validate();

    std::cout << "Dimensions: " << data->GetDimensionCount() << std::endl;

    if (cmvn)
    {
        std::cout << "Normalizing (CMVN)..." << std::endl;
        data->Normalize();
    }
}

std::string GetSpeakerString(unsigned int index, const std::string& folder)
{
#ifdef INDEXFIX
    if (folder.size() == 0)
    {
        std::cout << "Could not load samples: Missing folder name." << std::endl;
        return "";
    }

    std::stringstream ss(folder);

    std::string finalFolder;

    if (!std::getline(ss, finalFolder, '_'))
    {
        std::cout << "Invalid folder name." << std::endl;
        return "";
    }

    static std::vector<std::string> speakerFiles;
    static bool speakerFilesScanned = false;

    if (!speakerFilesScanned)
    {
        for (unsigned int i = 1; i <= 109; ++i)
        {
            std::string fname = finalFolder + "/samples_" + toString(i) + ".txt";

            std::ifstream infile(fname);

            if (infile.good())
            {
                std::string line;
                getline(infile, line);

                if (line.size() >= 3)
                {
                    std::string id;
                    id += line[0];
                    id += line[1];
                    id += line[2];

                    speakerFiles.push_back(id);

                    std::cout << "Scanned: " << i << ":" << id << std::endl;
                }
                else
                {
                    std::cout << "Scanning failed: insufficient data." << std::endl;
                }
            }
        }

        speakerFilesScanned = true;
    }

    return speakerFiles[index - 1];

#else
    return toString(index);
#endif
}
