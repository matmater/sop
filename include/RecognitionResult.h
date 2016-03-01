#ifndef _RECOGNITIONRESULT_H_
#define _RECOGNITIONRESULT_H_

#include "Common.h"

#include "SpeechData.h"

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
    RecognitionResult(const bool known = false, const SpeakerKey& speaker = SpeakerKey());

    ~RecognitionResult();

    Type GetType() const;

    const SpeakerKey& GetSpeaker() const;

    bool GetKnown() const;

private:
    bool mKnown;

    Type mType;

    SpeakerKey mSpeaker;
};

#endif
