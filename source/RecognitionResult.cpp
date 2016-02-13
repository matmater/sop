#include "RecognitionResult.h"

RecognitionResult::RecognitionResult(const bool known, const SpeakerKey& speaker)
    : mKnown(known), mSpeaker(speaker)
{

}

RecognitionResult::~RecognitionResult()
{

}

bool RecognitionResult::GetKnown() const
{
    return mKnown;
}

const SpeakerKey& RecognitionResult::GetSpeaker() const
{
    return mSpeaker;
}
