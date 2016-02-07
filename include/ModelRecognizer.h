#ifndef _MODELRECOGNIZER_H_
#define _MODELRECOGNIZER_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

#include "Recognizer.h"
#include "Model.h"
#include "Utilities.h"

enum class NormalizationType
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
    ModelRecognizer();

    virtual ~ModelRecognizer();

    virtual void ClearTrainedData();

    void SetScoreNormalizationType(NormalizationType type);

    NormalizationType GetScoreNormalizationType() const;

    void SetBackgroundModelEnabled(bool enabled);

    bool IsBackgroundModelEnabled() const;

    void SetOrder(unsigned int order);

    unsigned int GetOrder() const;

    virtual void SetImpostorSpeakerData(std::shared_ptr<SpeechData> data);

    virtual std::shared_ptr<SpeechData> GetImpostorSpeakerData();

    //virtual void SetBackgroundModelData(std::shared_ptr<SpeechData> data);

    //virtual std::shared_ptr<SpeechData> GetBackgroundModelData();

    virtual void Train(const std::shared_ptr<SpeechData>& data) override;

    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<std::string, RecognitionResult>& results) override;
    
    virtual Real GetVerificationScore(const std::string& speaker, const std::vector< DynamicVector<Real> >& samples);

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
    std::shared_ptr<T> mBackgroundModel;
    
    std::shared_ptr<SpeechData> mImpostorSpeakerData;
    std::shared_ptr<SpeechData> mBackgroundModelData;

    std::map< std::string, std::shared_ptr<T> > mSpeakerModels;
    std::map< std::string, std::shared_ptr<T> > mImpostorModels;

    std::map< std::string, Distribution> mImpostorDistributions;

    NormalizationType mScoreNormalizationType;

    bool mBackgroundModelEnabled;

    unsigned int mOrder;
};

#include "ModelRecognizer.inl"

#endif
