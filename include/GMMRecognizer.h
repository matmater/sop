#ifndef _GMMRECOGNIZER_H_
#define _GMMRECOGNIZER_H_

#include "SpeechData.h"
#include "RecognitionResult.h"
#include "ModelRecognizer.h"

class GMMRecognizer : public ModelRecognizer
{
public:
    GMMRecognizer();

    virtual ~GMMRecognizer();

    virtual void Train();

    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results) override;

protected:
    virtual std::shared_ptr<Model> CreateModel();
};

#endif
