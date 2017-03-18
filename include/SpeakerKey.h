/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _SPEAKERKEY_H_
#define _SPEAKERKEY_H_

#include "Common.h"

/*! \class SpeakerKey
 *  \brief Identifies speakers by their ids.
 *
 *  Raw format: speakerid_sampleid
 */
class SpeakerKey
{
public:
    /*! \brief Construct the key from a string.
     *
     *  \param id Raw speaker id string.
     */
    explicit SpeakerKey(const std::string& id = "");

    /*! \brief Get raw id string.
     *
     *  \return Raw speaker id string.
     */
    std::string GetId() const;

    /*! \brief Default less operator.
     *
     *  \param rhs Right-hand-side operand.
     *
     *  \return True if lhs.id < rhs.id, false otherwise.
     */
    bool operator< (const SpeakerKey& rhs) const;

    /*! \brief Default equality operator.
     *
     *  \param rhs Right-hand-side operand.
     *
     *  \return True if lhs.id == rhs.id, false otherwise.
     */
    bool operator== (const SpeakerKey& rhs) const;

    /*! \brief Default inequality operator.
     *
     *  \param rhs Right-hand-side operand.
     *
     *  \return True if lhs.id != rhs.id, false otherwise.
     */
    bool operator!= (const SpeakerKey& rhs) const;

    /*! \brief Check if another speaker key match this
     *  speaker key, excluding sample id.
     *
     *  \param rhs Right-hand-side operand (the another speaker key).
     *
     *  \return True if the actual speaker ids match.
     *
     *  \todo Fix.
     */
    bool IsSameSpeaker(const SpeakerKey& rhs) const;

    /*! \brief Stream operator to stream raw speaker id string.
     *
     *  \param stream The output stream.
     *  \param key The speaker key to be printed.
     *
     *  \return The reference to the output stream.
     */
    friend std::ostream& operator<< (std::ostream& stream, const SpeakerKey& key);

private:
    std::string mId;
};

#endif
