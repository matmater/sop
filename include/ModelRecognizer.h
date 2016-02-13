#ifndef _MODELRECOGNIZER_H_
#define _MODELRECOGNIZER_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

#include "Recognizer.h"
#include "Model.h"
#include "Utilities.h"

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
 */
template<typename T>
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
    
    /*! \brief Sets background model data.
     *
     *  This takes effect when the model is trained and the background model is enabled.
     *  See SetBackgroundModelEnabled().
     */
    virtual void SetBackgroundModelData(std::shared_ptr<SpeechData> data);
    
    /*! \brief Gets background model data.
     */
    virtual std::shared_ptr<SpeechData> GetBackgroundModelData();
    
    /*! \brief Sets impostor speaker data.
     *
     *  Impostor speaker data can be any speaker data. In case impostor data is
     *  equal to training data no duplicate models will be trained. Also it is guaranteed
     *  that the speakers that are also in impostor data are not used as their impostors.
     */
    virtual void SetImpostorSpeakerData(std::shared_ptr<SpeechData> data);

    /*! \brief Returns impostor speaker data.
     */
    virtual std::shared_ptr<SpeechData> GetImpostorSpeakerData();
    
    /*! \brief Enables/disables speaker models to be used also as impostors.
     *
     *  Disabled by default.
     */
    void SetSpeakerImpostorsEnabled(bool enabled);
    
    /*! \brief Checks if speaker models are used also as impostors.
     */
    bool IsSpeakerImpostorsEnabled() const;

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
    
    /*! \brief Gets the relevance factor used for model adaptation.
     */
    Real GetRelevanceFactor() const;

    /*! \copydoc Recognizer::Train()
     */
    virtual void Train(const std::shared_ptr<SpeechData>& data) override;
    
    /*! \copydoc Recognizer::Test()
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<std::string, RecognitionResult>& results) override;
    
    /*! \brief Calculates a verification score for a claimed speaker based on given samples.
     *
     *  This calculates the final verification score:
     *  If the background model is enabled and available the calculated speaker/ubm ratio will be returned.
     *  Additionally, if normalization is enabled the ratio will be normalized.
     *
     *  If the background model is not available the raw score of the speaker will be returned and normalized
     *  if required.
     */
    virtual Real GetVerificationScore(const std::string& speaker, const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Returns verification scores of multiple samples.
     *
     *  \sa GetVerificationScore()
     */
    virtual std::vector<Real> GetMultipleVerificationScore(const std::string& speaker, const std::shared_ptr<SpeechData>& data);
    
    /*! \copydoc Recognizer::Verify()
     */
    virtual std::vector<Real> Verify(const std::string& speaker, const std::shared_ptr<SpeechData>& data) override;

protected:
    /*! \brief Post-process models after training.
     */
    virtual void PostProcessModels();
    
    /*! \brief Unnormalized version of GetMultipleVerificationScore().
     */
    virtual Real GetRatio(const std::shared_ptr<T>& model, const std::vector< DynamicVector<Real> >& samples);

    virtual std::shared_ptr<T> GetBackgroundModel();

    virtual void SetBackgroundModel(std::shared_ptr<T> model);
    
    std::map< std::string, std::shared_ptr<T> >& GetImpostorModels();
    
    const std::map< std::string, std::shared_ptr<T> >& GetImpostorModels() const;

    std::map< std::string, std::shared_ptr<T> >& GetSpeakerModels();
    
    const std::map< std::string, std::shared_ptr<T> >& GetSpeakerModels() const;

    virtual unsigned int GetDimensionCount() const;

private:
    unsigned int mOrder;

    unsigned int mAdaptationIterations;

    Real mRelevanceFactor;
    
    ScoreNormalizationType mScoreNormalizationType;
    
    bool mBackgroundModelEnabled;
    
    bool mSpeakerImpostorsEnabled;

    std::shared_ptr<T> mBackgroundModel;
    
    std::shared_ptr<SpeechData> mImpostorSpeakerData;
    std::shared_ptr<SpeechData> mTrainedImpostorSpeakerData;

    std::shared_ptr<SpeechData> mBackgroundModelData;
    std::shared_ptr<SpeechData> mTrainedBackgroundModelData;

    std::map< std::string, std::shared_ptr<T> > mSpeakerModels;
    std::map< std::string, std::shared_ptr<T> > mImpostorModels;

    std::map< std::string, Distribution> mImpostorDistributions;
};

#include "ModelRecognizer.inl"

#endif
