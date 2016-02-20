#ifndef _MODELRECOGNIZER_H_
#define _MODELRECOGNIZER_H_

#include "Common.h"

#include "SpeechData.h"

#include "RecognitionResult.h"

#include "Recognizer.h"
#include "Model.h"

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
 *  \todo Add missing functionalities.
 *  \todo Add/fix comments.
 *  \todo Check normalization.
 *  \todo Check correctness.
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
    
    /*! \brief Clears all trained data.
     */
    virtual void ClearTrainedData();

    /*! \brief Sets score normalization type.
     */
    void SetScoreNormalizationType(ScoreNormalizationType type);
    
    /*! \brief Gets score normalization type.
     */
    ScoreNormalizationType GetScoreNormalizationType() const;

    /*! \brief Sets background model enabled or disabled.
     *  
     *  This takes effect whenever the model is trained. If the background
     *  is enabled background model data must be provided by calling SetBackgroundModel().
     */
    void SetBackgroundModelEnabled(bool enabled);
    
    /*! \brief Checks if the background model is enabled.
     */
    bool IsBackgroundModelEnabled() const;
    
    void SetBackgroundModelTrainingEnabled(bool enabled);

    bool IsBackgroundModelTrainingEnabled() const;

    /*! \brief Sets background model data.
     *
     *  This takes effect when the model is trained and the background model is enabled.
     *  See SetBackgroundModelEnabled().
     */
    virtual void SetBackgroundModelData(std::shared_ptr<SpeechData> data);
    
    /*! \brief Gets background model data.
     */
    virtual std::shared_ptr<SpeechData> GetBackgroundModelData();
    
    /*! \brief !!!!!!!!!!!!
     */
    virtual void SetSpeakerData(std::shared_ptr<SpeechData> data);

    /*! \brief !!!!!!!!!!!!
     */
    virtual std::shared_ptr<SpeechData> GetSpeakerData();

    /*! \brief Sets the order of this model.
     *
     *  The order is a synonym here for cluster count.
     */
    void SetOrder(unsigned int order);
    
    /*! \brief Returns the order of this model.
     *
     *  The order is a synonym here for cluster count.
     */
    unsigned int GetOrder() const;
    
    void SetAdaptationEnabled(bool enabled);

    bool IsAdaptationEnabled() const;

    /*! \brief Sets the number of iterations used for model adaptation.
     *
     *  Default number of iterations is 2.
     */
    void SetAdaptationIterations(unsigned int iterations);
    
    /*! \brief Gets the number of iterations used for model adaptation.
     */
    unsigned int GetAdaptationIterations() const;

    /*! \brief Sets the relevance factor used for model adaptation.
     *
     *  Default relevance factor is 16.
     */
    void SetRelevanceFactor(Real factor);
    
    /*! \brief Sets the (maximum) number of iterations used in training.
     */
    void SetTrainingIterations(unsigned int iterations);
    
    /*! \brief Sets the (maximum) number of iterations used in training.
     */
    unsigned int GetTrainingIterations() const;

    /*! \brief Sets the training threshold used in training as a stopping condition.
     */
    void SetTrainingThreshold(Real threshold);
    
    /*! \brief Sets the training threshold used in training as a stopping condition.
     */
    Real GetTrainingThreshold() const;

    /*! \brief Gets the relevance factor used for model adaptation.
     */
    Real GetRelevanceFactor() const;

    /*! \copydoc Recognizer::Train()
     */
    virtual void Train() override;

    virtual void SelectSpeakerModels(const std::vector<SpeakerKey>& models);

    virtual void SelectImpostorModels(const std::vector<SpeakerKey>& models);
    
    /*! \copydoc Recognizer::Test()
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results) override;
    
    virtual void Prepare();

    /*! \brief Checks if a given speaker is recognized.
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

    /*! \brief Calculates a verification score for a claimed speaker based on given samples.
     *
     *  This calculates the final verification score:
     *  If the background model is enabled and available the calculated speaker/ubm ratio will be returned.
     *  Additionally, if normalization is enabled the ratio will be normalized.
     *
     *  If the background model is not available the raw score of the speaker will be returned and normalized
     *  if required.
     */
    virtual Real GetVerificationScore(const SpeakerKey& speaker, const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Returns verification scores of multiple samples.
     *
     *  \sa GetVerificationScore()
     */
    virtual std::vector<Real> GetMultipleVerificationScore(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data);
    
    /*! \copydoc Recognizer::Verify()
     */
    virtual std::vector<Real> Verify(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data) override;

protected:
    void TrainBackgroundModel();

    void TrainSpeakerModels();

    virtual std::shared_ptr<Model> CreateModel() = 0;
    
    /*! \brief Post-process models after training.
     */
    virtual void PrepareModels();
    
    virtual void Unprepare();

    /*! \brief Unnormalized version of GetMultipleVerificationScore().
     */
    virtual Real GetRatio(const std::shared_ptr<Model>& model, const std::vector< DynamicVector<Real> >& samples);

    virtual std::shared_ptr<Model> GetBackgroundModel();

    virtual void SetBackgroundModel(std::shared_ptr<Model> model);
    
    std::map<SpeakerKey, std::shared_ptr<Model> >& GetImpostorModels();
    
    const std::map<SpeakerKey, std::shared_ptr<Model> >& GetImpostorModels() const;

    std::map<SpeakerKey, std::shared_ptr<Model> >& GetSpeakerModels();
    
    const std::map<SpeakerKey, std::shared_ptr<Model> >& GetSpeakerModels() const;

    virtual unsigned int GetDimensionCount();

private:
    unsigned int mOrder;

    unsigned int mAdaptationIterations;

    Real mRelevanceFactor;
    
    ScoreNormalizationType mScoreNormalizationType;
    
    bool mBackgroundModelEnabled;

    bool mBackgroundModelTrainingEnabled;

    bool mDirty;

    bool mPrepared;
    
    bool mAdaptationEnabled;
    
    unsigned int mTrainingIterations;
    
    Real mEta;
    
    unsigned int mBackgroundModelDirty;

    unsigned int mSpeakerModelsDirty;

    std::shared_ptr<Model> mBackgroundModel;
    
    std::map<SpeakerKey, std::shared_ptr<Model> > mModelCache;

    std::shared_ptr<SpeechData> mSpeakerData;

    std::shared_ptr<SpeechData> mBackgroundModelData;

    std::map<SpeakerKey, std::shared_ptr<Model> > mSpeakerModels;
    std::map<SpeakerKey, std::shared_ptr<Model> > mImpostorModels;

    std::map<SpeakerKey, Distribution> mImpostorDistributions;
};

#endif
