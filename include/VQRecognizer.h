#ifndef _VQ_H_
#define _VQ_H_

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

    /*! \copydoc Recognizer::Train()
     */
    virtual void Train(const std::shared_ptr<SpeechData>& data) override;

    /*! \copydoc Recognizer::SaveTrainedData()
     */
    virtual void SaveTrainedData(const std::string& path) override;

    /*! \copydoc Recognizer::LoadTrainedData()
     */
    virtual void LoadTrainedData(const std::string& path) override;

    /*! \copydoc Recognizer::Test()
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<std::string, RecognitionResult>& results) override;

    /*! \copydoc Recognizer::Verify()
     */
    virtual std::vector<Real> Verify(const std::string& speaker, const std::shared_ptr<SpeechData>& data) override;
    };

#endif
