template<typename T>
ModelRecognizer<T>::ModelRecognizer()
:   mOrder(128),
    mAdaptationIterations(2),
    mRelevanceFactor(16.0f),
    mScoreNormalizationType(ScoreNormalizationType::NONE),
    mBackgroundModelEnabled(false),
    mSpeakerImpostorsEnabled(false)
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
    mImpostorDistributions.clear();
    //mImpostorModels.clear();
    //mBackgroundModel = nullptr;
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
void ModelRecognizer<T>::SetAdaptationIterations(unsigned int iterations)
{
    mAdaptationIterations = iterations;
}

template<typename T>
unsigned int ModelRecognizer<T>::GetAdaptationIterations() const
{
    return mAdaptationIterations;
}

template<typename T>
void ModelRecognizer<T>::SetRelevanceFactor(Real factor)
{
    mRelevanceFactor = factor;
}

template<typename T>
Real ModelRecognizer<T>::GetRelevanceFactor() const
{
    return mRelevanceFactor;
}

template<typename T>
void ModelRecognizer<T>::SetScoreNormalizationType(ScoreNormalizationType type)
{
    mScoreNormalizationType = type;
}

template<typename T>
ScoreNormalizationType ModelRecognizer<T>::GetScoreNormalizationType() const
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
void ModelRecognizer<T>::SetSpeakerImpostorsEnabled(bool enabled)
{
    mSpeakerImpostorsEnabled = enabled;
}
    
template<typename T>
bool ModelRecognizer<T>::IsSpeakerImpostorsEnabled() const
{
    return mSpeakerImpostorsEnabled;
}

template<typename T>
void ModelRecognizer<T>::PostProcessModels()
{
    // Virtual
}

template<typename T>
void ModelRecognizer<T>::SetBackgroundModelData(std::shared_ptr<SpeechData> data)
{
    mBackgroundModelData = data;
}

template<typename T>
std::shared_ptr<SpeechData> ModelRecognizer<T>::GetBackgroundModelData()
{
    return mBackgroundModelData;
}

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

    // Check if the background model can actually be created.
    if (IsBackgroundModelEnabled() && mBackgroundModelData != nullptr)
    {
        if (mTrainedBackgroundModelData != mBackgroundModelData)
        {
            mBackgroundModel = std::make_shared<T>();

            // UBM was found, train it first using normal training.
            std::cout << "Training background model." << std::endl;

            std::vector< DynamicVector<Real> > samples;
        
            for (const auto& speaker : mBackgroundModelData->GetSamples())
            {
                for (const auto& sample : speaker.second)
                {
                    samples.push_back(sample);
                }
            }

            mBackgroundModel->SetOrder(GetOrder());
            mBackgroundModel->Train(samples);

            mTrainedBackgroundModelData = mBackgroundModelData;
        }

        else
        {
            std::cout << "Using an already trained background model." << std::endl;
        }
    }

    progress = 0;

    std::cout << "Training speaker models." << std::endl;

    for (const auto& sequence : data->GetSamples())
    {
        ++progress;

        auto model = std::make_shared<T>();
        mSpeakerModels[sequence.first] = model;

        std::cout << "Training model: " << sequence.first
                    << " (" << 100 * progress / data->GetSamples().size() << "%)" << std::endl;

        // UBM exists, train everything else with adaptation.
        if (IsBackgroundModelEnabled() && mBackgroundModel != nullptr)
        {
            model->Adapt(mBackgroundModel, sequence.second, mAdaptationIterations, mRelevanceFactor);
        }

        // No UBM, train normally.
        else
        {
            model->SetOrder(GetOrder());

            model->Train(sequence.second);
        }
    }

    PostProcessModels();

    progress = 0;

    // Train T-norm impostor models.
    if (   (mScoreNormalizationType == ScoreNormalizationType::TEST
         || mScoreNormalizationType == ScoreNormalizationType::ZERO_TEST
         || mScoreNormalizationType == ScoreNormalizationType::TEST_ZERO)
         && mImpostorSpeakerData != nullptr)
    {
        std::cout << "Training impostor models." << std::endl;

        // If one happens to use same speaker data for impostors.
        if (mImpostorSpeakerData == data)
        {
            mImpostorModels.clear();

            std::cout << "Using existing models." << std::endl;

            // Set models for T-normalization.
            for (auto& speakerModel : mSpeakerModels)
            {
                mImpostorModels[speakerModel.first] = speakerModel.second;
            }
        }

        else
        {
            if (mTrainedImpostorSpeakerData != mImpostorSpeakerData)
            {
                mImpostorModels.clear();

                // Train models for T-normalization.
                for (const auto& sequence : mImpostorSpeakerData->GetSamples())
                {
                    ++progress;

                    auto model = std::make_shared<T>();
                    mImpostorModels[sequence.first] = model;

                    std::cout << "Training model: " << sequence.first
                        << " (" << 100 * progress / mImpostorSpeakerData->GetSamples().size() << "%)" << std::endl;

                    // UBM exists, train everything else with adaptation.
                    if (IsBackgroundModelEnabled() && mBackgroundModel != nullptr)
                    {
                        model->Adapt(mBackgroundModel, sequence.second, mAdaptationIterations, mRelevanceFactor);
                    }

                    // No UBM, train normally.
                    else
                    {
                        model->SetOrder(GetOrder());

                        model->Train(sequence.second);
                    }
                }

                mTrainedImpostorSpeakerData = mImpostorSpeakerData;
            }

            else
            {
                std::cout << "Using already trained impostor models." << std::endl;
            }
        }
    }

    // Z norm
    if (   mScoreNormalizationType == ScoreNormalizationType::ZERO
        || mScoreNormalizationType == ScoreNormalizationType::ZERO_TEST
        || mScoreNormalizationType == ScoreNormalizationType::TEST_ZERO)
    {
        if (mImpostorSpeakerData == nullptr && !mSpeakerImpostorsEnabled)
        {
            std::cout << "Missing impostor speaker data for normalization." << std::endl;
        }

        else
        {
            progress = 0;

            std::cout << "Calculating Z-norm scores using: ";
            
            std::cout << "impostors, ";

            if (mSpeakerImpostorsEnabled)
            {
                std::cout << "speakers";
            }

            std::cout << "." << std::endl;

            for (auto& model : mSpeakerModels)
            {
                ++progress;

                std::cout << "Calculating Z-norm scores: " << model.first
                    << " (" << 100 * progress / mSpeakerModels.size() << "%)" << std::endl;

                std::vector<Real> scores;
                
                unsigned int impostors = 0;

                // Z-norm scores.
                for (auto& impostor : mImpostorSpeakerData->GetSamples())
                {
                    if (impostor.first != model.first)
                    {
                        scores.push_back(GetRatio(model.second, impostor.second));

                        ++impostors;
                    }
                }
                
                if (mSpeakerImpostorsEnabled)
                {
                    for (auto& impostor : data->GetSamples())
                    {
                        // Already checked.
                        if (mImpostorSpeakerData->GetSamples().find(impostor.first)
                            != mImpostorSpeakerData->GetSamples().end())
                        {
                            continue;
                        }

                        if (impostor.first != model.first)
                        {
                            scores.push_back(GetRatio(model.second, impostor.second));

                            ++impostors;
                        }
                    }
                }

                if (impostors > 1)
                {
                    // Initialize speaker-specific Z-normalization parameters.
                    auto& zd = mImpostorDistributions[model.first];
                
                    zd.mean = Mean(scores);
                    zd.deviation = Deviation(scores, zd.mean);
                }

                else
                {
                    std::cout << "Not enough impostors for Z-normalization was found." << std::endl;
                }
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

    // Return score immediately if normalization not enabled.
    if (mScoreNormalizationType == ScoreNormalizationType::NONE)
    {
        return score;
    }

    Distribution zd;
    Distribution td;
    
    // Get Z-norm parameters.
    if (   mScoreNormalizationType == ScoreNormalizationType::ZERO
        || mScoreNormalizationType == ScoreNormalizationType::ZERO_TEST
        || mScoreNormalizationType == ScoreNormalizationType::TEST_ZERO)
    {
        zd = mImpostorDistributions[it->first];
    }

    // Calculate T-norm parameters.
    if (   mScoreNormalizationType == ScoreNormalizationType::TEST
        || mScoreNormalizationType == ScoreNormalizationType::ZERO_TEST
        || mScoreNormalizationType == ScoreNormalizationType::TEST_ZERO)
    {
        std::vector<Real> scores;

        unsigned int impostors = 0;

        for (auto& impostor : mImpostorModels)
        {
            if (impostor.first == speaker)
            {
                continue;
            }
            
            ++impostors;

            scores.push_back(GetRatio(impostor.second, samples));
        }

        if (mSpeakerImpostorsEnabled)
        {
            for (auto& impostor : mSpeakerModels)
            {
                // Already checked.
                if (mImpostorModels.find(impostor.first)
                    != mImpostorModels.end())
                {
                    continue;
                }

                if (impostor.first == speaker)
                {
                    continue;
                }
            
                ++impostors;

                scores.push_back(GetRatio(impostor.second, samples));
            }
        }

        if (impostors > 1)
        {
            td.mean = Mean(scores);
            td.deviation = Deviation(scores, td.mean);
        }
        
        else
        {
            std::cout << "Not enough impostors for T-normalization was found." << std::endl;
        }
    }
    
    // Apply normalization.
    switch (mScoreNormalizationType)
    {
    case ScoreNormalizationType::ZERO:
        return (score - zd.mean) / zd.deviation;

    case ScoreNormalizationType::TEST:
        return (score - td.mean) / td.deviation;

    case ScoreNormalizationType::ZERO_TEST:
        return (((score - zd.mean) / zd.deviation) - td.mean) / td.deviation;

    case ScoreNormalizationType::TEST_ZERO:
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
        std::cout << "Incompatible testing data dimensions: " << GetDimensionCount() << " " << data->GetDimensionCount() << std::endl;

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
        results.push_back(GetVerificationScore(speaker, entry.second));
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
