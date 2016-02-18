#ifndef _GMMRECOGNIZER_H_
#define _GMMRECOGNIZER_H_

#include "SpeechData.h"
#include "RecognitionResult.h"
#include "ModelRecognizer.h"

/*! \brief A speaker recognizer based on Gaussian Mixture Models.
 *
 *  \todo Add/fix comments.
 */
class GMMRecognizer : public ModelRecognizer
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
    virtual void Train();

    /*! \copydoc Recognizer::Test()
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results) override;

protected:
    virtual std::shared_ptr<Model> CreateModel();
};

#endif
