#ifndef _MODELRECOGNIZER_H_
#define _MODELRECOGNIZER_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

#include "Recognizer.h"
#include "Model.h"
#include "Utilities.h"

/*! \brief A generic model-based speaker recognizer.
 *
 *  \todo Add missing functionalities.
 *  \todo Add/fix comments.
 */
template<typename T>
class ModelRecognizer : public Recognizer
{
public:
    enum class NormalizationType
    {
        Z_NORM,
        T_NORM,
        ZT_NORM
    };

public:
    ModelRecognizer();

    virtual ~ModelRecognizer();

    void SetScoreNormalizationType(NormalizationType type);

    NormalizationType GetScoreNormalizationType() const;

    void SetBackgroundModelEnabled(bool enabled);

    bool IsBackgroundModelEnabled() const;

    void SetOrder(unsigned int order);

    unsigned int GetOrder() const;

    virtual void SetImpostorSpeakerData(std::shared_ptr<SpeechData> data);

    virtual std::shared_ptr<SpeechData> GetImpostorSpeakerData();

    virtual void SetBackgroundModelData(std::shared_ptr<SpeechData> data);

    virtual std::shared_ptr<SpeechData> GetBackgroundModelData();

    virtual void Train(const std::shared_ptr<SpeechData>& data) override;

    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<std::string, RecognitionResult>& results) override;

    virtual Real GetVerificationScore(const std::string& speaker, const std::vector< DynamicVector<Real> >& samples);

    virtual std::vector<Real> GetMultipleVerificationScore(const std::string& speaker, const std::shared_ptr<SpeechData>& data);

protected:
    virtual std::shared_ptr<T> GetBackgroundModel();

    virtual void SetBackgroundModel(std::shared_ptr<T> model);

    virtual std::shared_ptr<T> RegisterSpeakerModel(const std::string& name);

    virtual std::shared_ptr<T> GetSpeakerModel(const std::string& name);

    std::map< std::string, std::shared_ptr<T> >& GetSpeakerModels();

    virtual void RemoveAllSpeakerModels();

    virtual unsigned int GetDimensionCount() const;

private:
    std::shared_ptr<T> mBackgroundModel;

    std::map< std::string, std::shared_ptr<T> > mSpeakerModels;

    NormalizationType mScoreNormalizationType;

    Real mTrainingThreshold;

    unsigned int mMaxTrainingIterations;

    bool mBackgroundModelEnabled;

    unsigned int mOrder;

    std::shared_ptr<SpeechData> mImpostorSpeakerData;
    std::shared_ptr<SpeechData> mBackgroundModelData;
};

#include "ModelRecognizer.inl"

#endif
