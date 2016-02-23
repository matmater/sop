#ifndef _SPEECHDATA_H_
#define _SPEECHDATA_H_

#include "Common.h"

#include "DynamicVector.h"

#include "SpeakerKey.h"

#include <streambuf>

class SpeechData;

// \todo MOVE
void LoadTextSamples(const std::string& folder, const std::shared_ptr<SpeechData>& data, unsigned int sf, unsigned int gf, unsigned int sl, unsigned int gl, bool train);

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

    /*! \brief Loads data from text file. (Old version).
     */
    void Load(const std::string& path);
    
    /*! \brief Loads data from text file(s)
     *
     *  \param sl Start line.
     *  \param gl Number of lines to fetch.
     *  \param train If this flag is true, test specific labels will be removed.
     *
     *  \note Line indexing starts from 1.
     */
    void Load(const std::string& path, unsigned int sl, unsigned int gl, bool train, unsigned int maxFeatures, bool normalize = false, const std::string& alias = "");

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
     */
    bool IsConsistent() const;

    /*! \brief Checks if two speech data sets are consistent (compatible).
     *
     *  \sa IsConsistent().
     */
    bool IsCompatible(const SpeechData& other);

    /*! \brief Returns the dimension count.
     *
     *  If data is inconsistent the returned value is undefined.
     */
    unsigned int GetDimensionCount() const;
    
    void CMVN(
        std::vector < DynamicVector<Real> >::iterator beginIt,
        std::vector < DynamicVector<Real> >::iterator endIt);

    void Normalize(
        std::vector < DynamicVector<Real> >::iterator beginIt,
        std::vector < DynamicVector<Real> >::iterator endIt);

    /*! \brief Normalizes all sample values to range x.
     *
     *  \todo Check range ([0,1] or [-1,1]).
     */
    void Normalize();
    
    /*! \brief Sets the feature normalization type.
     */
    void SetNormalizationType(FeatureNormalizationType normalizationType);
    
    /*! \brief Gets the feature normalization type.
     */
    FeatureNormalizationType GetNormalizationType() const;

    /*! \brief Returns the number of loaded speakers.
     */
    unsigned int GetSpeakerCount() const;

    /*! \brief Returns the total number of speech samples over all speakers.
     */
    unsigned int GetTotalSampleCount() const;

    /*! \brief Returns all samples identified by speaker strings.
     */
    const std::map<SpeakerKey, std::vector<DynamicVector<Real> > >& GetSamples() const;

private:
    FeatureNormalizationType mNormalizationType;

    std::map<SpeakerKey, std::vector<DynamicVector<Real> > > mSamples;

    bool mConsistent;

    unsigned int mDimensionCount;

    unsigned int mTotalSampleCount;
};

#endif
