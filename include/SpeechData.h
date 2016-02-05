#ifndef _SPEECHDATA_H_
#define _SPEECHDATA_H_

#include "Common.h"

#include "DynamicVector.h"

/*! \brief A container for speech data of multiple speakers.
 *
 *  \todo Add/fix comments.
 */
class SpeechData
{
public:
    /*! \brief Default constructor.
     */
    SpeechData();

    /*! \brief Virtual destructor.
     */
    virtual ~SpeechData();

    /*! \brief Clears old data and loads new data.
     */
    void Load(const std::string& path);

    /*! \brief Validates loaded data.
     *
     *  Checks that data dimensions match.
     */
    void Validate();

    /*! \brief Clears all loaded data.
     */
    void Clear();

    /*! \brief Checks if data is consistent.
     *
     *  Data is considered consistet if all speaker
     *  samples have same number of dimensions.
     */
    bool IsConsistent() const;

    /*! \brief Checks if two speech data sets are consistent (compatible).
     *
     *  \see IsConsistent().
     */
    bool IsCompatible(const SpeechData& other);

    /*! \brief Returns the dimension count.
     *
     *  If data is inconsistent the returned value is undefined.
     */
    unsigned int GetDimensionCount() const;

    /*! \brief Normalizes all sample values to range x.
     *
     *  \todo Check range ([0,1] or [-1,1]).
     */
    void Normalize();

    /*! \brief Returns the number of loaded speakers.
     */
    unsigned int GetSpeakerCount() const;

    /*! \brief Returns the total number of speech samples of all speakers.
     */
    unsigned int GetTotalSampleCount() const;

    /*! \brief Returns all samples identified by speaker strings.
     */
    const std::map<std::string, std::vector<DynamicVector<Real> > >& GetSamples() const;

private:
    std::map<std::string, std::vector<DynamicVector<Real> > > mSamples;

    bool mConsistent;

    unsigned int mDimensionCount;

    unsigned int mTotalSampleCount;
};

#endif
