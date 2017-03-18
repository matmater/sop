/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _SPEECHDATA_H_
#define _SPEECHDATA_H_

#include "Common.h"

#include "DynamicVector.h"

#include "SpeakerKey.h"

#include <streambuf>

class SpeechData;

void LoadTextSamples(const std::string& folder, const std::shared_ptr<SpeechData>& data, unsigned int sf, unsigned int gf, unsigned int sl, unsigned int gl, unsigned int multiplier, bool train);

std::string GetSpeakerString(unsigned int index, const std::string& folder);

struct SpeechDataBuffer : public std::streambuf
{
    SpeechDataBuffer(char* b, std::size_t s)
    {
        setg(b, b, b + s);
    }
};

enum class FeatureNormalizationType
{
    NONE = 0,
    CEPSTRAL_MEAN_VARIANCE
};

/*! \class SpeechData
 *  \brief A container for speech data of multiple speakers.
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

    /*! \brief Loads speech data from a text file. (Old version).
     *
     *  \param path Path to a file containing speaker-specific speech data.
     */
    void Load(const std::string& path);

    /*! \brief Loads speech data from a text file(s)
     *
     *  \param sl Start line.
     *  \param gl Number of lines to fetch.
     *  \param multiplier Tells how many lines are considered as a single line
     *  (combine multiple utterances).
     *  \param train If this flag is true, test specific labels will be removed.
     *  \param maxFeatures Maxmimum number of features to load (from the
     *  beginning of the feature vector).
     *  \param normalize Enable utterance-based normalization.
     *  \param alias Alter the speaker id.
     *
     *  \note Line indexing starts at 1.
     */
    void Load(const std::string& path, unsigned int sl, unsigned int gl,
        unsigned multiplier, bool train, unsigned int maxFeatures,
        bool normalize = false, const std::string& alias = "");

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
     *  Data is considered consistent if all speaker
     *  samples have the same number of dimensions.
     *
     *  \return True if speech data is consistent, false otherwise.
     */
    bool IsConsistent() const;

    /*! \brief Checks if two speech data sets are consistent (compatible).
     *
     *  \return True If two data sets are compatible with each other,
     *  false otherwise.
     *
     *  \see IsConsistent().
     */
    bool IsCompatible(const SpeechData& other);

    /*! \brief Returns the dimension count.
     *
     *  \return The number of feature dimensions.
     *
     *  \note If data is inconsistent the returned value is undefined.
     */
    unsigned int GetDimensionCount() const;

    /*! \brief Normalize selected samples using Ceptstral Mean Variance
     *  Normalization.
     *
     *  \param beginIt Begin iterator of the samples to be normalized.
     *  \param endIt End iterator of the samples to be normalized.
     */
    void CMVN(
        std::vector < DynamicVector<Real> >::iterator beginIt,
        std::vector < DynamicVector<Real> >::iterator endIt);

    /*! \brief Normalize selected samples using a predefined normalization type.
    *
     *  \param beginIt Begin iterator of the samples to be normalized.
     *  \param endIt End iterator of the samples to be normalized.
     */
    void Normalize(
        std::vector < DynamicVector<Real> >::iterator beginIt,
        std::vector < DynamicVector<Real> >::iterator endIt);

    /*! \brief Normalize all samples.
     */
    void Normalize();

    /*! \brief Sets the feature normalization type.
     *
     *  \param normalizationType The feature normalization type.
     */
    void SetNormalizationType(FeatureNormalizationType normalizationType);

    /*! \brief Gets the feature normalization type.
     *
     *  \return The feature normalization type.
     */
    FeatureNormalizationType GetNormalizationType() const;

    /*! \brief Returns the number of loaded speakers.
     *
     *  \return The number of loaded speakers.
     */
    unsigned int GetSpeakerCount() const;

    /*! \brief Returns the total number of speech samples over all speakers.
     *
     *  \return The total number of all samples.
     */
    unsigned int GetTotalSampleCount() const;

    /*! \brief Returns all samples identified by speaker keys.
     *
     *  \return A map containing all loaded speaker keys and respective samples.
     */
    const std::map<SpeakerKey, std::vector<DynamicVector<Real> > >&
    GetSamples() const;

private:
    FeatureNormalizationType mNormalizationType;

    std::map<SpeakerKey, std::vector<DynamicVector<Real> > > mSamples;

    bool mConsistent;

    unsigned int mDimensionCount;

    unsigned int mTotalSampleCount;
};

#endif
