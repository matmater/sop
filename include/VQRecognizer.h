#ifndef _VQRECOGNIZER_H_
#define _VQRECOGNIZER_H_

#include "SpeechData.h"

#include "Common.h"

#include "ModelRecognizer.h"
#include "DynamicVector.h"
#include "LBG.h"
#include "VQModel.h"

/*! \brief A speaker recognizer based on Vector Quantization.
 */
class VQRecognizer : public ModelRecognizer
{
public:
    VQRecognizer();

    virtual ~VQRecognizer();

    void SetWeightingEnabled(bool enabled);

    bool IsWeightingEnabled() const;

    virtual void Train() override;

    virtual void PrepareModels();

    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results) override;

protected:
    virtual std::shared_ptr<Model> CreateModel();

private:
    bool mWeightingEnabled;
};

#endif
