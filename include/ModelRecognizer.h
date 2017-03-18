/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _MODELRECOGNIZER_H_
#define _MODELRECOGNIZER_H_

#include "Common.h"

#include "SpeechData.h"

#include "RecognitionResult.h"

#include "Recognizer.h"
#include "Model.h"
#include "Timer.h"

enum class ScoreNormalizationType
{
    NONE,
    ZERO,
    TEST,
    ZERO_TEST,
    TEST_ZERO
};

/*! \brief A generic model-based speaker recognizer.
 *
 *  \todo Check normalization.
 */
class ModelRecognizer : public Recognizer
{
public:
    struct Distribution
    {
        Real mean;
        Real deviation;
    };

public:
    /*! \brief Default constructor.
     */
    ModelRecognizer();

    /*! \brief Virtual destructor.
     */
    virtual ~ModelRecognizer();

    /*! \brief Clear all trained data (models, cache etc.).
     */
    virtual void ClearTrainedData();

    /*! \brief Set the normalization type for scores.
     *
     *  \param type The score normalization type.
     */
    void SetScoreNormalizationType(ScoreNormalizationType type);

    /*! \brief Get the normalization type for scores.
     *
     *  \return The score normalization type.
     */
    ScoreNormalizationType GetScoreNormalizationType() const;

    /*! \brief Set background model enabled or disabled.
     *
     *  This takes effect whenever the model is trained. If the background model
     *  is enabled background model data must be provided by calling SetBackgroundModelData().
     *
     *  \param enabled True to enable, false to disable.
     */
    void SetBackgroundModelEnabled(bool enabled);

    /*! \brief Check if the background model is enabled.
     *  (not necessarily trained).
     *
     *  \return True if the background model enabled flag
     *  is true, false otherwise.
     */
    bool IsBackgroundModelEnabled() const;

    /*! \brief Enable or disable background model training.
     *
     *  \param enabled True to enable training, false to disable.
     *
     *  \see SetBackgroundModelEnabled()
     */
    void SetBackgroundModelTrainingEnabled(bool enabled);

    /*! \brief Check if the background model training is enabled.
     *
     *  \return True if enabled, false otherwise.
     */
    bool IsBackgroundModelTrainingEnabled() const;

    /*! \brief Set background model data.
     *
     *  This takes effect when the model is trained and the background model is enabled.
     *  See SetBackgroundModelEnabled().
     *
     *  \param data The speech data set for training containing
     *  background model speakers and their samples.
     */
    virtual void SetBackgroundModelData(std::shared_ptr<SpeechData> data);

    /*! \brief Get background model data.
     *
     *  \return Pointer to background model data (not necessarily used in
     *  training yet).
     */
    virtual std::shared_ptr<SpeechData> GetBackgroundModelData();

    /*! \brief Set speaker speech data.
     *
     *  \param data Speaker speech data for training.
     */
    virtual void SetSpeakerData(std::shared_ptr<SpeechData> data);

    /*! \brief Get speaker speech data.
     *
     *  \return Pointer to speaker speech data (not necessarily used in
     *  training yet).
     */
    virtual std::shared_ptr<SpeechData> GetSpeakerData();

    /*! \brief Set the order of this model.
     *
     *  The order is a synonym here for cluster count.
     *
     *  \param order The order: 2, 4, 8, 16, 32, ...
     */
    void SetOrder(unsigned int order);

    /*! \brief Get the order of this model.
     *
     *  The order is a synonym here for cluster count.
     *
     *  \return The order: 2, 4, 8, 16, 32, ...
     */
    unsigned int GetOrder() const;

    /*! \brief Enable or disable adaptation.
     *
     *  \param enabled True to enable, false to disable.
     */
    void SetAdaptationEnabled(bool enabled);

    /*! \brief Check if adaptation is enabled.
     *
     *  \return True if adaptation is enabled, false otherwise.
     */
    bool IsAdaptationEnabled() const;

    /*! \brief Set the number of adaptation iterations to use (if
     *  the models are trained through adaptation).
     *
     *  \param iterations The number of iterations.
     */
    void SetAdaptationIterations(unsigned int iterations);

    /*! \brief Get the number of adaptation iterations to use (if
     *  the models are trained through adaptation).
     *
     * \return The number adaptation iterations.
     */
    unsigned int GetAdaptationIterations() const;

    /*! \brief Set the relevance factor to adapt the models.
     *
     *  \param factor The relevance factor.
     */
    void SetRelevanceFactor(Real factor);

    /*! \brief Get the relevance factor to adapt the models.
     *
     *  \return The relevance factor.
     */
    Real GetRelevanceFactor() const;

    /*! \brief Set the maximum number of normal training iterations.
     *
     *  \param iterations The iteration limit.
     */
    void SetTrainingIterations(unsigned int iterations);

    /*! \brief Get the maximum number of normal training iterations.
     *
     *  \return The iteration limit.
     */
    unsigned int GetTrainingIterations() const;

    /*! \brief Set the training threshold used as a stopping condition.
     *
     *  \param The training threshold.
     */
    void SetTrainingThreshold(Real threshold);

    /*! \brief Get the training threshold used as a stopping condition.
     *
     *  \return The training threshold.
     */
    Real GetTrainingThreshold() const;

    /*! \brief Train all speaker models and the background model (if enabled).
     */
    virtual void Train() override;

    /*! \brief Select certain models as the active speaker models involved in
     *  recognition and verification.
     *
     *  \param models Active speaker keys.
     */
    virtual void SelectSpeakerModels(const std::vector<SpeakerKey>& models);

    /*! \brief Select certain models as the active impostor models involved in
     *  recognition and verification.
     *
     *  \param models Active speaker keys.
     */
    virtual void SelectImpostorModels(const std::vector<SpeakerKey>& models);

    /*! \brief Get the time duration spent in background model training.
     *
     *  \return Time duration in seconds.
     */
    Real GetTrainTimeBackgroundModel();

    /*! \brief Get the time duration spent in speaker model training.
     *
     *  \return Time duration in seconds.
     */
    Real GetTrainTimeSpeakerModels();

    /*! \brief Check if a given speaker is recognized.
     *
     *  All speaker models are checked against given samples. The model that gives the highest
     *  score will be chosen as the recognized speaker.
     *
     *  \param speaker Correct speaker.
     *  \param samples Correct speaker samples to be tested.
     *
     *  \return True if the recognized speaker and the given speaker match. False, otherwise.
     */
    virtual bool IsRecognized(const SpeakerKey& speaker, const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Calculate a verification score for a claimed speaker based on given samples.
     *
     *  This calculates the final verification score:
     *  If the background model is enabled and available the calculated speaker/ubm ratio will be returned.
     *  Additionally, if normalization is enabled the ratio will be normalized.
     *
     *  If the background model is not available the raw score of the speaker will be returned and normalized
     *  if required.
     */
    virtual Real GetVerificationScore(const SpeakerKey& speaker, const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Return verification scores of multiple samples.
     *
     *  \param speaker The speaker to be verified.
     *  \param data Speech data set to derive verification scores.
     *
     *  \return A vector containing verification scores of the speaker.
     *
     *  \sa GetVerificationScore()
     */
    virtual std::vector<Real> GetMultipleVerificationScore(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data);

    /*! \brief Verify the claimed speaker.
     *
     *  \param speaker The speaker to be verified.
     *  \param data Speech data set to derive verification scores.
     *
     *  \return A vector containing verification scores of the speaker.
     */
    virtual std::vector<Real> Verify(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data) override;

protected:
    /*! \brief Prepare calculations after training.
     */
    virtual void Prepare();

    /*! \brief Train the background model (forced).
     */
    void TrainBackgroundModel();

    /*! \brief Train the speaker models using normal training
     *  or adaptation (forced).
     */
    void TrainSpeakerModels();

    /*! \brief Create a speaker model (abstract).
     */
    virtual std::shared_ptr<Model> CreateModel() = 0;

    /*! \brief Post-process models after training.
     */
    virtual void PrepareModels();

    /*! \brief Invalidate calculations.
     *
     *  \see Prepare()
     */
    virtual void Unprepare();

    /*! \brief Unnormalized version of the GetMultipleVerificationScore().
     *
     *  \param speaker The speaker to be verified.
     *  \param data Speech data set to derive verification scores.
     *
     *  \return Unnormalized verification score.
     */
    virtual Real GetRatio(const std::shared_ptr<Model>& model, const std::vector< DynamicVector<Real> >& samples);

    /* \brief Get the trained background model.
     *
     * \return Pointer to the trained background model, nullptr otherwise.
     */
    virtual std::shared_ptr<Model> GetBackgroundModel();

    /*! \brief Set the trained background model.
     *
     *  \param model The background model. Nullptr indicates an untrained model.
     */
    virtual void SetBackgroundModel(std::shared_ptr<Model> model);

    /*! \brief Non-const version to get all active impostor models.
     *
     *  \return A map containing impostor speaker keys and respective models.
     */
    std::map<SpeakerKey, std::shared_ptr<Model> >& GetImpostorModels();

    /*! \brief Const version to get all active impostor models.
     *
     *  \return A map containing impostor speaker keys and respective models.
     */
    const std::map<SpeakerKey, std::shared_ptr<Model> >& GetImpostorModels() const;

    /*! \brief Non-const version to get all active speaker models.
     *
     *  \return A map containing speaker keys and respective models.
     */
    std::map<SpeakerKey, std::shared_ptr<Model> >& GetSpeakerModels();

    /*! \brief Const version to get all active speaker models.
     *
     *  \return A map containing speaker keys and respective models.
     */
    const std::map<SpeakerKey, std::shared_ptr<Model> >& GetSpeakerModels() const;

    /*! \brief Get the number of feature dimensions.
     *
     *  \return The number of feature dimensions.
     */
    virtual unsigned int GetDimensionCount();

private:
    unsigned int mOrder;

    unsigned int mAdaptationIterations;

    Real mRelevanceFactor;

    ScoreNormalizationType mScoreNormalizationType;

    bool mBackgroundModelEnabled;

    bool mDirty;

    bool mPrepared;

    bool mAdaptationEnabled;

    unsigned int mTrainingIterations;

    Real mEta;

    unsigned int mBackgroundModelDirty;

    unsigned int mSpeakerModelsDirty;

    Real mTrainTimeBackgroundModel;

    Real mTrainTimeSpeakerModels;

    std::shared_ptr<Model> mBackgroundModel;

    std::map<SpeakerKey, std::shared_ptr<Model> > mModelCache;

    std::shared_ptr<SpeechData> mSpeakerData;

    std::shared_ptr<SpeechData> mBackgroundModelData;

    std::map<SpeakerKey, std::shared_ptr<Model> > mSpeakerModels;
    std::map<SpeakerKey, std::shared_ptr<Model> > mImpostorModels;

    std::map<SpeakerKey, Distribution> mImpostorDistributions;
};

#endif
