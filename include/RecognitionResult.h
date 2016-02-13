#ifndef _RECOGNITIONRESULT_H_
#define _RECOGNITIONRESULT_H_

#include "Common.h"

#include "SpeechData.h"

/*! \brief The result of a speaker recognition task.
 *
 *  \todo Add/fix comments.
 */
class RecognitionResult
{
public:
    enum class Type
    {
        RECOGNIZED = 0,
        UNRECOGNIZED,
        UNKNOWN
    };

public:
    /*! \brief Constructor with parameters.
     */
    RecognitionResult(const bool known = false, const SpeakerKey& speaker = SpeakerKey());

    /*! \brief Virtual destructor.
     */
    ~RecognitionResult();

    /*! \brief Returns the type of the recognition.
     */
    Type GetType() const;

    /*! \brief Returns the string identifier of the speaker.
     */
    const SpeakerKey& GetSpeaker() const;

    /*! A flag that tells if the speaker existed in database.
     */
    bool GetKnown() const;

private:
    bool mKnown;

    Type mType;

    SpeakerKey mSpeaker;
};

#endif
