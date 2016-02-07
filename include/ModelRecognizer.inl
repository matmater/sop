template<typename T>
ModelRecognizer<T>::ModelRecognizer()
: mScoreNormalizationType(NormalizationType::NONE)
{

}

template<typename T>
ModelRecognizer<T>::~ModelRecognizer()
{

}

template<typename T>
void ModelRecognizer<T>::ClearTrainedData()
{
    mSpeakerModels.clear();
    mImpostorModels.clear();
    mImpostorDistributions.clear();
    mBackgroundModel = nullptr;
}

template<typename T>
void ModelRecognizer<T>::SetOrder(unsigned int order)
{
    mOrder = order;
}

template<typename T>
unsigned int ModelRecognizer<T>::GetOrder() const
{
    return mOrder;
}

template<typename T>
void ModelRecognizer<T>::SetScoreNormalizationType(NormalizationType type)
{
    mScoreNormalizationType = type;
}

template<typename T>
typename NormalizationType ModelRecognizer<T>::GetScoreNormalizationType() const
{
    return mScoreNormalizationType;
}

template<typename T>
void ModelRecognizer<T>::SetBackgroundModelEnabled(bool enabled)
{
    mBackgroundModelEnabled = enabled;
}

template<typename T>
bool ModelRecognizer<T>::IsBackgroundModelEnabled() const
{
    return mBackgroundModelEnabled;
}

template<typename T>
void ModelRecognizer<T>::SetImpostorSpeakerData(std::shared_ptr<SpeechData> data)
{
    mImpostorSpeakerData = data;
}

template<typename T>
std::shared_ptr<SpeechData> ModelRecognizer<T>::GetImpostorSpeakerData()
{
    return mImpostorSpeakerData;
}

template<typename T>
void ModelRecognizer<T>::PostProcessModels()
{

}

//template<typename T>
//void ModelRecognizer<T>::SetBackgroundModelData(std::shared_ptr<SpeechData> data)
//{
//    mBackgroundModelData = data;
//}
//
//template<typename T>
//std::shared_ptr<SpeechData> ModelRecognizer<T>::GetBackgroundModelData()
//{
//    return mBackgroundModelData;
//}

template<typename T>
void ModelRecognizer<T>::Train(const std::shared_ptr<SpeechData>& data)
{
    if (!data->IsConsistent())
    {
        std::cout << "Inconsistent training data." << std::endl;

        return;
    }

    ClearTrainedData();

    unsigned int progress = 0;

    // Try to find UBM from samples
    auto ubmIt = data->GetSamples().find(".ubm");

    // If UBM was not found, it will return map.
    if (IsBackgroundModelEnabled())
    {
        mBackgroundModel = std::make_shared<T>();

        if (ubmIt == data->GetSamples().end())
        {
            std::cout << "Missing ubm data." << std::endl;

            return;
        }
    }

    if (IsBackgroundModelEnabled())
    {
        // UBM was found, train it first using normal training.
        std::cout << "Training background model." << std::endl;

        mBackgroundModel->SetOrder(GetOrder());
        mBackgroundModel->Train(ubmIt->second);

        ++progress;
    }

    progress = 0;
    
    std::cout << "Training speaker models." << std::endl;
    for (const auto& sequence : data->GetSamples())
    {
        if (sequence.first == ".ubm")
        {
            continue;
        }
        
        auto model = std::make_shared<T>();
        mSpeakerModels[sequence.first] = model;
        
        std::cout << "Training model: " << sequence.first
                    << " (" << 100 * progress / data->GetSamples().size() << "%)" << std::endl;

        // UBM exists, train everything else with adaptation.
        if (IsBackgroundModelEnabled() && mBackgroundModel != nullptr)
        {
            model->Adapt(mBackgroundModel, sequence.second);
        }

        // No UBM, train normally.
        else
        {
            model->SetOrder(GetOrder());

            model->Train(sequence.second);
        }
        
        ++progress;
    }
    
    PostProcessModels();

    progress = 0;

    // T, Z norm
    if (   (mScoreNormalizationType == NormalizationType::TEST
         || mScoreNormalizationType == NormalizationType::ZERO_TEST
         || mScoreNormalizationType == NormalizationType::TEST_ZERO)
         && mImpostorSpeakerData != nullptr)
    {
        std::cout << "Training impostor models." << std::endl;

        // If one happens to use same speaker data for impostors.
        if (mImpostorSpeakerData == data)
        {
            std::cout << "Using existing models." << std::endl;

            for (auto& speakerModel : mSpeakerModels)
            {
                mImpostorModels[speakerModel.first] = speakerModel.second;
            }
        }

        else
        {
            for (const auto& sequence : mImpostorSpeakerData->GetSamples())
            {
                if (sequence.first == ".ubm")
                {
                    continue;
                }

                auto model = std::make_shared<T>();
                mImpostorModels[sequence.first] = model;

                std::cout << "Training model: " << sequence.first
                    << " (" << 100 * progress / mImpostorSpeakerData->GetSamples().size() << "%)" << std::endl;

                // UBM exists, train everything else with adaptation.
                if (IsBackgroundModelEnabled() && mBackgroundModel != nullptr)
                {
                    model->Adapt(mBackgroundModel, sequence.second);
                }

                // No UBM, train normally.
                else
                {
                    model->SetOrder(GetOrder());

                    model->Train(sequence.second);
                }

                ++progress;
            }
        }

    }

    // Z norm
    if (   mScoreNormalizationType == NormalizationType::ZERO
        || mScoreNormalizationType == NormalizationType::ZERO_TEST
        || mScoreNormalizationType == NormalizationType::TEST_ZERO)
    {
        if (mImpostorSpeakerData == nullptr)
        {
            std::cout << "Missing impostor speaker data." << std::endl;
        }

        else
        {
            for (auto& model : mSpeakerModels)
            {
                std::vector<Real> scores;

                // Using same data for now..
                for (auto& impostor : mImpostorSpeakerData->GetSamples())
                {
                    scores.push_back(GetRatio(model.second, impostor.second));
                }

                auto& zd = mImpostorDistributions[model.first];
            
                zd.mean = Mean(scores);
                zd.deviation = Deviation(scores, zd.mean);
            }
        }
    }
}

template<typename T>
void ModelRecognizer<T>::Test(const std::shared_ptr<SpeechData>& data, std::map<std::string, RecognitionResult>& results)
{
    if (!data->IsConsistent())
    {
        std::cout << "Inconsistent testing data." << std::endl;

        return;
    }

    if (GetDimensionCount() != data->GetDimensionCount())
    {
        std::cout << "Incompatible testing data dimensions." << std::endl;

        return;
    }

    if (IsBackgroundModelEnabled() && mBackgroundModel == nullptr)
    {
        std::cout << "Background model not created." << std::endl;

        return;
    }

    // Clear old results (if any).
    results.clear();

    for (auto& entry : data->GetSamples())
    {
        std::string bestModelName = "";
        std::shared_ptr<T> bestModel = nullptr;

        bool knownSpeaker = false;
        Real bestModelScore = std::numeric_limits<Real>::min();

        for (auto& model : mSpeakerModels)
        {
            if (Utilities::IsSameSpeaker(entry.first, model.first))
            {
                knownSpeaker = true;
            }

            // Do not confuse these!
            Real modelScore = model.second->GetScore(entry.second);
            Real modelLogScore = model.second->GetLogScore(entry.second);

            // Notice logarithmic domain.
            Real ubmLogScore = std::numeric_limits<Real>::max();

            if (mBackgroundModel != nullptr && mBackgroundModelEnabled)
            {
                ubmLogScore = mBackgroundModel->GetLogScore(entry.second);
            }

            DEBUG_TRACE(entry.first << "-" << model.first
                << " m:" << modelScore
                << ",l:" << modelLogScore
                << ",u:" << ubmLogScore
                << ",r:" << modelLogScore - ubmLogScore
                << ",v:" << GetVerificationScore(model.first, entry.second));

            if (modelScore > bestModelScore)
            {
                bestModelScore = modelScore;
                bestModel = model.second;
                bestModelName = model.first;
            }
        }

        DEBUG_TRACE("The most probable speaker is " << bestModelName << ".");

        if (bestModel != nullptr)
        {
            // Checking for nullptr ensures that UBM is enabled
            // and the model actually exists.
            if (mBackgroundModel != nullptr)
            {
                Real logRatio = bestModel->GetLogScore(entry.second) - mBackgroundModel->GetLogScore(entry.second);

                // UBM is not too close to bestMatch.
                if (logRatio > 0.3f && bestModelScore >= 0.08f) // UBM Threshold
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
                if (bestModelScore >= 0.135f)
                {
                    results[entry.first] = RecognitionResult(knownSpeaker, bestModelName);
                }

                else
                {
                    results[entry.first] = RecognitionResult(knownSpeaker);
                }
            }
        }

        // Should happen only if there are no models trained. Still unrecognized?
        else
        {
            results[entry.first] = RecognitionResult(knownSpeaker);
        }
    }
}

template<typename T>
Real ModelRecognizer<T>::GetRatio(const std::shared_ptr<T>& model, const std::vector< DynamicVector<Real> >& samples)
{
    if (IsBackgroundModelEnabled() && (mBackgroundModel != nullptr))
    {
        return model->GetLogScore(samples) - mBackgroundModel->GetLogScore(samples);
    }

    return model->GetScore(samples);
}

template<typename T>
Real ModelRecognizer<T>::GetVerificationScore(const std::string& speaker, const std::vector< DynamicVector<Real> >& samples)
{
    auto it = mSpeakerModels.find(speaker);

    if (it == mSpeakerModels.end())
    {
        std::cout << "Speaker model '" << speaker << "' not found." << std::endl;
        return 0.0f;
    }

    Real score = GetRatio(it->second, samples);

    if (mScoreNormalizationType == NormalizationType::NONE)
    {
        return score;
    }
    
    Distribution zd;
    Distribution td;
    
    if (   mScoreNormalizationType == NormalizationType::ZERO
        || mScoreNormalizationType == NormalizationType::ZERO_TEST
        || mScoreNormalizationType == NormalizationType::TEST_ZERO)
    {
        zd = mImpostorDistributions[it->first];
    }

    if (   mScoreNormalizationType == NormalizationType::TEST
        || mScoreNormalizationType == NormalizationType::ZERO_TEST
        || mScoreNormalizationType == NormalizationType::TEST_ZERO)
    {
        std::vector<Real> scores;

        for (auto& impostor : mImpostorModels)
        {
            if (impostor.first == it->first)
            {
                continue;
            }

            scores.push_back(GetRatio(impostor.second, samples));
        }

        td.mean = Mean(scores);
        td.deviation = Deviation(scores, td.mean);
    }

    switch (mScoreNormalizationType)
    {
    case NormalizationType::ZERO:
        return (score - zd.mean) / zd.deviation;
        
    case NormalizationType::TEST:
        return (score - td.mean) / td.deviation;
        
    case NormalizationType::ZERO_TEST:
        return (((score - zd.mean) / zd.deviation) - td.mean) / td.deviation;

    case NormalizationType::TEST_ZERO:
        return (((score - td.mean) / td.deviation) - zd.mean) / zd.deviation;

    default:
        std::cout << "Unknown score normalization type." << std::endl;
        return 0.0f;
    }
}

template<typename T>
std::vector<Real> ModelRecognizer<T>::GetMultipleVerificationScore(const std::string& speaker, const std::shared_ptr<SpeechData>& data)
{
    std::vector<Real> results;
    if (!data->IsConsistent())
    {
        std::cout << "Inconsistent testing data." << std::endl;

        return results;
    }

    if (GetDimensionCount() != data->GetDimensionCount())
    {
        std::cout << "Incompatible testing data dimensions." << std::endl;

        return results;
    }

    if (IsBackgroundModelEnabled() && mBackgroundModel == nullptr)
    {
        std::cout << "Background model not created." << std::endl;

        return results;
    }

    auto it = mSpeakerModels.find(speaker);

    if (it == mSpeakerModels.end())
    {
        std::cout << "Speaker model '" << speaker << "' not found." << std::endl;
        return results;
    }

    for (auto& entry : data->GetSamples())
    {
        if (IsBackgroundModelEnabled() && (mBackgroundModel != nullptr))
        {
            results.push_back(it->second->GetLogScore(entry.second) - mBackgroundModel->GetLogScore(entry.second));
        }

        results.push_back(it->second->GetScore(entry.second));
    }
    return results;
}

template<typename T>
std::vector<Real> ModelRecognizer<T>::Verify(const std::string& speaker, const std::shared_ptr<SpeechData>& data)
{
    return GetMultipleVerificationScore(speaker, data);
}

template<typename T>
unsigned int ModelRecognizer<T>::GetDimensionCount() const
{
    if (mSpeakerModels.size() == 0)
    {
        return 0;
    }

    unsigned int speakerModelDimensions = mSpeakerModels.begin()->second->GetDimensionCount();

    if (IsBackgroundModelEnabled() && (mBackgroundModel == nullptr || speakerModelDimensions != mBackgroundModel->GetDimensionCount()))
    {
        std::cout << "Mismatching speaker model and background model dimensions." << std::endl;
        return -1;
    }

    return speakerModelDimensions;
}

template<typename T>
void ModelRecognizer<T>::SetBackgroundModel(std::shared_ptr<T> model)
{
    mBackgroundModel = model;
}

template<typename T>
std::shared_ptr<T> ModelRecognizer<T>::GetBackgroundModel()
{
    return mBackgroundModel;
}

template<typename T>
std::map< std::string, std::shared_ptr<T> >& ModelRecognizer<T>::GetImpostorModels()
{
    return mImpostorModels;
}
    
template<typename T>
const std::map< std::string, std::shared_ptr<T> >& ModelRecognizer<T>::GetImpostorModels() const
{
    return mImpostorModels;
}

template<typename T>
const std::map< std::string, std::shared_ptr<T> >& ModelRecognizer<T>::GetSpeakerModels() const
{
    return mSpeakerModels;
}

template<typename T>
std::map< std::string, std::shared_ptr<T> >& ModelRecognizer<T>::GetSpeakerModels()
{
    return mSpeakerModels;
}