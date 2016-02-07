#ifndef _VQRECOGNIZER_H_
#define _VQRECOGNIZER_H_

#include "SpeechData.h"

#include "Common.h"

#include "DynamicVector.h"
#include "LBG.h"
#include "ModelRecognizer.h"
#include "VQModel.h"

/*! \brief A speaker recognizer based on Vector Quantization.
 *
 *  \todo Add/fix comments.
 */
class VQRecognizer : public ModelRecognizer<VQModel>
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
    virtual void Train(const std::shared_ptr<SpeechData>& data) override;

    virtual void PostProcessModels();

    /*! \copydoc Recognizer::SaveTrainedData()
     */
    virtual void SaveTrainedData(const std::string& path) override;

    /*! \copydoc Recognizer::LoadTrainedData()
     */
    virtual void LoadTrainedData(const std::string& path) override;

    /*! \copydoc Recognizer::Test()
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<std::string, RecognitionResult>& results) override;

private:
    bool mWeightingEnabled;
};

#endif
