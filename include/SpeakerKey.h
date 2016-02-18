#ifndef _SPEAKERKEY_H_
#define _SPEAKERKEY_H_

#include "Common.h"

/*! A speaker-specific key.
 */
class SpeakerKey
{
public:
    explicit SpeakerKey(const std::string& id = "");

    std::string GetId() const;

    bool operator< (const SpeakerKey& rhs) const;

    bool operator== (const SpeakerKey& rhs) const;

    bool operator!= (const SpeakerKey& rhs) const;

    bool IsSameSpeaker(const SpeakerKey& rhs) const;

    friend std::ostream& operator<< (std::ostream& stream, const SpeakerKey& key);

private:
    std::string mId;
};

#endif
