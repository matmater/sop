#include "RecognitionResult.h"

/*
RecognitionResult::RecognitionResult(Type type, const std::string& speaker)
	: mType(type), mSpeaker(speaker)
{

}
*/

RecognitionResult::RecognitionResult(const bool known, const std::string& speaker)
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

const std::string& RecognitionResult::GetSpeaker() const
{
	return mSpeaker;
}