#ifndef _RECOGNIZER_H_
#define _RECOGNIZER_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

/*! \brief The main structure of a speaker recognizer.
 *
 *  \todo Add/fix comments.
 */
class Recognizer
{
public:
    /*! \brief Virtual destructor.
     */
    virtual ~Recognizer() { }

    /*! \brief Trains the recognizer with given speech data.
     */
    virtual void Train() = 0;

    /*! \brief Tests samples on the trained recognizer.
     *
     *  \todo Remove?
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results) {}

    /*! \brief Verifies samples on the trained recognizer.
     */
    virtual std::vector<Real> Verify(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data) = 0;
};

#endif
