#include "SpeakerKey.h"

SpeakerKey::SpeakerKey(const std::string& id)
: mId(id)
{

}

std::string SpeakerKey::GetId() const
{
    return mId;
}

bool SpeakerKey::operator< (const SpeakerKey& rhs) const
{
    return mId < rhs.mId;
}

bool SpeakerKey::operator== (const SpeakerKey& rhs) const
{
    return mId == rhs.mId;
}

bool SpeakerKey::operator!= (const SpeakerKey& rhs) const
{
    return mId != rhs.mId;
}

bool SpeakerKey::IsSameSpeaker(const SpeakerKey& rhs) const
{
    return (mId.compare(0, 3, rhs.mId) == 0);
}

std::ostream& operator<< (std::ostream& stream, const SpeakerKey& key)
{
    stream << key.mId;

    return stream;
}