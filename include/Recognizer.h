/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _RECOGNIZER_H_
#define _RECOGNIZER_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

/*! \brief Abstract speaker recognizer.
 */
class Recognizer
{
public:
    /*! \brief Virtual destructor.
     */
    virtual ~Recognizer() { }

    /*! \brief Train the recognizer.
     */
    virtual void Train() = 0;

    /*! \brief Check if the speaker was recognized.
     *
     *  \param speaker The speaker to be recognized.
     *  \param samples The samples of the speaker.
     *
     *  \return True if the speaker was recognized, false otherwise.
     */
    virtual bool IsRecognized(const SpeakerKey& speaker, const std::vector< DynamicVector<Real> >& samples) = 0;

    /*! \brief Verify the claimed speaker.
     *
     *  \param speaker The speaker to be verified.
     *  \param data Speech data set to derive verification scores.
     *
     *  \return A vector containing verification scores of the speaker.
     */
    virtual std::vector<Real> Verify(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data) = 0;
};

#endif
