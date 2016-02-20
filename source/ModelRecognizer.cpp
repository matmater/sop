#include "ModelRecognizer.h"

ModelRecognizer::ModelRecognizer()
:   mOrder(128),
    mAdaptationIterations(2),
    mRelevanceFactor(16.0f),
    mScoreNormalizationType(ScoreNormalizationType::NONE),
    mBackgroundModelEnabled(false),
    mBackgroundModelTrainingEnabled(true),
    mDirty(true),
    mPrepared(false),
    mTrainingIterations(10),
    mEta(0.001f),
    mBackgroundModelDirty(true),
    mAdaptationEnabled(false),
    mSpeakerModelsDirty(true)
{

}

ModelRecognizer::~ModelRecognizer()
{

}

void ModelRecognizer::ClearTrainedData()
{
    mPrepared = false;
    mModelCache.clear();
    mSpeakerModels.clear();
    mImpostorDistributions.clear();
    mImpostorModels.clear();
    mDirty = true;
    mBackgroundModel = nullptr;
}

void ModelRecognizer::SetOrder(unsigned int order)
{
    mOrder = order;
    mDirty = true;
}

unsigned int ModelRecognizer::GetOrder() const
{
    return mOrder;
}

void ModelRecognizer::SetAdaptationEnabled(bool enabled)
{
    if (enabled != mAdaptationEnabled)
    {
        mSpeakerModelsDirty = true;
    }

    mAdaptationEnabled = enabled;
}

bool ModelRecognizer::IsAdaptationEnabled() const
{
    return mAdaptationEnabled;
}

void ModelRecognizer::SetAdaptationIterations(unsigned int iterations)
{
    mAdaptationIterations = iterations;
    mDirty = true;
}

unsigned int ModelRecognizer::GetAdaptationIterations() const
{
    return mAdaptationIterations;
}

void ModelRecognizer::SetRelevanceFactor(Real factor)
{
    mRelevanceFactor = factor;
    mDirty = true;
}

Real ModelRecognizer::GetRelevanceFactor() const
{
    return mRelevanceFactor;
}

void ModelRecognizer::SetScoreNormalizationType(ScoreNormalizationType type)
{
    mScoreNormalizationType = type;
}

ScoreNormalizationType ModelRecognizer::GetScoreNormalizationType() const
{
    return mScoreNormalizationType;
}

void ModelRecognizer::SetBackgroundModelEnabled(bool enabled)
{
    mBackgroundModelEnabled = enabled;
    mPrepared = false;
}

bool ModelRecognizer::IsBackgroundModelEnabled() const
{
    return mBackgroundModelEnabled;
}

void ModelRecognizer::SetBackgroundModelTrainingEnabled(bool enabled)
{
    mBackgroundModelTrainingEnabled = enabled;
}

bool ModelRecognizer::IsBackgroundModelTrainingEnabled() const
{
    return mBackgroundModelTrainingEnabled;
}

void ModelRecognizer::SetSpeakerData(std::shared_ptr<SpeechData> data)
{
    if (data != mSpeakerData)
    {
        mDirty = true;
    }

    mSpeakerData = data;
}

std::shared_ptr<SpeechData> ModelRecognizer::GetSpeakerData()
{
    return mSpeakerData;
}

void ModelRecognizer::Unprepare()
{
    mPrepared = false;
}

void ModelRecognizer::PrepareModels()
{
    // Virtual
}

void ModelRecognizer::SetBackgroundModelData(std::shared_ptr<SpeechData> data)
{
    if (data != mBackgroundModelData)
    {
        mDirty = true;
        mBackgroundModelDirty = true;
    }

    mBackgroundModelData = data;
}

std::shared_ptr<SpeechData> ModelRecognizer::GetBackgroundModelData()
{
    return mBackgroundModelData;
}

void ModelRecognizer::TrainBackgroundModel()
{
    mBackgroundModel = CreateModel();

    std::vector< DynamicVector<Real> > samples;
        
    for (const auto& speaker : mBackgroundModelData->GetSamples())
    {
        for (const auto& sample : speaker.second)
        {
            samples.push_back(sample);
        }
    }

    mBackgroundModel->SetOrder(GetOrder());
    mBackgroundModel->Train(samples, GetTrainingIterations());
}

void ModelRecognizer::TrainSpeakerModels()
{
    unsigned int progress = 0;
    
    bool adapt = false;

    if (IsBackgroundModelEnabled() && mBackgroundModel != nullptr && mAdaptationEnabled)
    {
        adapt = true;
    }

    else if (IsBackgroundModelEnabled() && mBackgroundModel == nullptr)
    {
        std::cout << "Warning: enabled background model not found." << std::endl;
    }

    for (const auto& sequence : mSpeakerData->GetSamples())
    {
        ++progress;

        auto model = CreateModel();
        mModelCache[sequence.first] = model;

        if (adapt)
        {
            std::cout << "Training model (MAP): " << sequence.first
                << " (" << 100 * progress / mSpeakerData->GetSamples().size() << "%)" << std::endl;
        }

        else
        {
            std::cout << "Training model: " << sequence.first
                << " (" << 100 * progress / mSpeakerData->GetSamples().size() << "%)" << std::endl;
        }

        // UBM exists, train everything else with adaptation.
        if (adapt)
        {
            model->Adapt(mBackgroundModel, sequence.second, mAdaptationIterations, mRelevanceFactor);
        }

        // No UBM, train normally.
        else
        {
            model->SetOrder(GetOrder());

            model->Train(sequence.second, GetTrainingIterations());
        }
    }
}

void ModelRecognizer::Train()
{
    if (mSpeakerData == nullptr || !mSpeakerData->IsConsistent())
    {
        std::cout << "Missing or inconsistent speaker model training data." << std::endl;
        return;
    }

    if (mBackgroundModelData != nullptr && !mSpeakerData->IsConsistent())
    {
        std::cout << "Inconsistent background model training data." << std::endl;
        return;
    }

    // Dirty means that something fundamental changes were made so that all models
    // need to be re-trained (i.e. the order).
    if (mDirty)
    {
        ClearTrainedData();
        
        std::cout << "Training background model." << std::endl;

        TrainBackgroundModel();

        std::cout << "Training speaker models." << std::endl;
        
        TrainSpeakerModels();
        
        mDirty = false;
        mBackgroundModelDirty = false;
        mSpeakerModelsDirty = false;
        mPrepared = false;
    }

    // Otherwise,
    else
    {
        // i.e., background model data changed.
        if (mBackgroundModelDirty)
        {
            std::cout << "Training background model." << std::endl;

            TrainBackgroundModel();

            mBackgroundModelDirty = false;
            mPrepared = false;
        }
        
        // i.e., if adaptation parameters changed.
        if (mSpeakerModelsDirty)
        {
            std::cout << "Training speaker models." << std::endl;

            TrainSpeakerModels();

            mSpeakerModelsDirty = false;
            mPrepared = false;
        }
    }
}

void ModelRecognizer::Prepare()
{
    // Is everything already OK?
    if (mPrepared)
    {
        return;
    }

    PrepareModels();

    unsigned int progress = 0;

    // Z norm
    if (   mScoreNormalizationType == ScoreNormalizationType::ZERO
        || mScoreNormalizationType == ScoreNormalizationType::ZERO_TEST
        || mScoreNormalizationType == ScoreNormalizationType::TEST_ZERO)
    {
        progress = 0;

        std::cout << "Calculating Z-norm scores." << std::endl;

        for (auto& model : mSpeakerModels)
        {
            ++progress;

            std::cout << "Calculating Z-norm scores: " << model.first
                << " (" << 100 * progress / mSpeakerModels.size() << "%)" << std::endl;

            std::vector<Real> scores;
                
            unsigned int impostors = 0;

            // Z-norm scores.
            for (auto& impostor : mImpostorModels)
            {
                auto it = mSpeakerData->GetSamples().find(impostor.first);

                if (it == mSpeakerData->GetSamples().end())
                {
                    std::cout << "Impostor speaker data not found." << std::endl;

                    continue;
                }

                if (impostor.first != model.first)
                {
                    scores.push_back(GetRatio(model.second, it->second));

                    ++impostors;
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

    mPrepared = true;
}

void ModelRecognizer::SelectSpeakerModels(const std::vector<SpeakerKey>& models)
{
    Train();

    mPrepared = false;

    mSpeakerModels.clear();

    for (const auto& key : models)
    {
        auto it = mModelCache.find(key);

        if (it == mModelCache.end())
        {
            std::cout << "Speaker model '" << key << "' could not be selected." << std::endl;
            continue;
        }

        mSpeakerModels[key] = it->second;
    }
}

void ModelRecognizer::SelectImpostorModels(const std::vector<SpeakerKey>& models)
{
    Train();

    mPrepared = false;

    mImpostorModels.clear();
    mImpostorDistributions.clear();
        
    for (const auto& key : models)
    {
        auto it = mModelCache.find(key);

        if (it == mModelCache.end())
        {
            std::cout << "Impostor model '" << key << "' could not be selected." << std::endl;
            continue;
        }

        mImpostorModels[key] = it->second;
    }
}

void ModelRecognizer::Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results)
{
    Train();
    Prepare();

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
        SpeakerKey bestModelName;
        std::shared_ptr<Model> bestModel = nullptr;

        bool knownSpeaker = false;
        Real bestModelScore = std::numeric_limits<Real>::min();

        for (auto& model : mSpeakerModels)
        {
            if (entry.first.IsSameSpeaker(model.first))
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

Real ModelRecognizer::GetRatio(const std::shared_ptr<Model>& model, const std::vector< DynamicVector<Real> >& samples)
{
    Train();

    if (IsBackgroundModelEnabled() && (mBackgroundModel != nullptr))
    {
        return model->GetLogScore(samples) - mBackgroundModel->GetLogScore(samples);
    }

    return model->GetScore(samples);
}

bool ModelRecognizer::IsRecognized(const SpeakerKey& speaker, const std::vector< DynamicVector<Real> >& samples)
{
    Train();

    Prepare();

    if (mSpeakerModels.empty())
    {
        std::cout << "Speaker not recognized: no speaker models found." << std::endl;

        return false;
    }

    Real bestScore = std::numeric_limits<Real>::min();
    SpeakerKey bestSpeaker;
    
    for (auto& model : mSpeakerModels)
    {
        Real score = model.second->GetScore(samples);

        if (score > bestScore)
        {
            bestScore = score;
            bestSpeaker = model.first;
        }
    }

    return bestSpeaker == speaker;
}

Real ModelRecognizer::GetVerificationScore(const SpeakerKey& speaker, const std::vector< DynamicVector<Real> >& samples)
{
    Train();

    Prepare();

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

std::vector<Real> ModelRecognizer::GetMultipleVerificationScore(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data)
{
    Train();

    Prepare();

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

std::vector<Real> ModelRecognizer::Verify(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data)
{
    Train();

    Prepare();

    return GetMultipleVerificationScore(speaker, data);
}

unsigned int ModelRecognizer::GetDimensionCount()
{
    Prepare();

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

void ModelRecognizer::SetBackgroundModel(std::shared_ptr<Model> model)
{
    mBackgroundModel = model;
}

std::shared_ptr<Model> ModelRecognizer::GetBackgroundModel()
{
    return mBackgroundModel;
}

std::map<SpeakerKey, std::shared_ptr<Model> >& ModelRecognizer::GetImpostorModels()
{
    return mImpostorModels;
}

const std::map<SpeakerKey, std::shared_ptr<Model> >& ModelRecognizer::GetImpostorModels() const
{
    return mImpostorModels;
}

const std::map<SpeakerKey, std::shared_ptr<Model> >& ModelRecognizer::GetSpeakerModels() const
{
    return mSpeakerModels;
}

std::map<SpeakerKey, std::shared_ptr<Model> >& ModelRecognizer::GetSpeakerModels()
{
    return mSpeakerModels;
}

void ModelRecognizer::SetTrainingIterations(unsigned int iterations)
{
    mTrainingIterations = iterations;
}

unsigned int ModelRecognizer::GetTrainingIterations() const
{
    return mTrainingIterations;
}

void ModelRecognizer::SetTrainingThreshold(Real threshold)
{
    mEta = threshold;
}

Real ModelRecognizer::GetTrainingThreshold() const
{
    return mEta;
}