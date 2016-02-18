#ifndef _VQRECOGNIZER_H_
#define _VQRECOGNIZER_H_

#include "SpeechData.h"

#include "Common.h"

#include "ModelRecognizer.h"
#include "DynamicVector.h"
#include "LBG.h"
#include "VQModel.h"

/*! \brief A speaker recognizer based on Vector Quantization.
 *
 *  \todo Add/fix comments.
 */
class VQRecognizer : public ModelRecognizer
{
public:
    /*! \brief Default constructor.
     */
    VQRecognizer();

    /*! \brief Virtual destructor.
     */
    virtual ~VQRecognizer();

    void SetWeightingEnabled(bool enabled);

    bool IsWeightingEnabled() const;

    /*! \copydoc Recognizer::Train()
     */
    virtual void Train() override;

    virtual void PrepareModels();

    /*! \copydoc Recognizer::Test()
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results) override;

protected:
    virtual std::shared_ptr<Model> CreateModel();

private:
    bool mWeightingEnabled;
};

#endif
