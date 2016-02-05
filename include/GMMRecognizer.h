#ifndef _GMMRECOGNIZER_H_
#define _GMMRECOGNIZER_H_

#include "SpeechData.h"
#include "RecognitionResult.h"
#include "ModelRecognizer.h"
#include "GMModel.h"

/*! \brief A speaker recognizer based on Gaussian Mixture Models.
 *
 *  \todo Add/fix comments.
 */
class GMMRecognizer : public ModelRecognizer<GMModel>
{
public:
    /*! \brief Default constructor.
     */
    GMMRecognizer();

    /*! \brief Virtual destructor.
     */
    virtual ~GMMRecognizer();

    /*! \copydoc Recognizer::Train()
     */
    virtual void Train(const std::shared_ptr<SpeechData>& data);

    /*! \copydoc Recognizer::SaveTrainedData()
     */
    virtual void SaveTrainedData(const std::string& path);

    /*! \copydoc Recognizer::LoadTrainedData()
     */
    virtual void LoadTrainedData(const std::string& path);

    /*! \copydoc Recognizer::Test()
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<std::string, RecognitionResult>& results) override;

private:
    std::map<std::string, GMModel> mModels;
};

#endif
