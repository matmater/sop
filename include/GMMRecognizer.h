#ifndef _GMMRECOGNIZER_H_
#define _GMMRECOGNIZER_H_

#include "GMM.h"
#include "SpeechData.h"
#include "RecognitionResult.h"
#include "Recognizer.h"

/*! \brief A speaker recognizer based on Gaussian Mixture Models.
 */
class GMMRecognizer : public Recognizer
{
public:
    /*! \brief Default constructor.
     */
    GMMRecognizer();

    /*! \brief Virtual destructor.
     */
    virtual ~GMMRecognizer();

    /*! \brief Enables/disables the background model.
     */
    void SetBackgroundModelEnabled(bool enabled);

    /*! \brief Checks if background model is enabled.
     */
    bool IsBackgroundModelEnabled() const;

    /*! \brief Sets the number of clusters.
     */
    void SetClusterCount(unsigned int clusterCount);
    
    /*! \brief Gets the number of clusters.
     */
    unsigned int GetClusterCount() const;

    /*! \copydoc Recognizer::Clear()
     */
    virtual void Clear();

    /*! \copydoc Recognizer::Train()
     */
    void Train(const SpeechData& data);

    /*! \copydoc Recognizer::SaveTrainedData()
     */
    void SaveTrainedData(const std::string& path);

    /*! \copydoc Recognizer::LoadTrainedData()
     */
    void LoadTrainedData(const std::string& path);

    /*! \copydoc Recognizer::Test()
     */
    void Test(const SpeechData& data, std::map<std::string, RecognitionResult>& results);

private:
    unsigned int mDimensionCount;

    Real mEta;

    bool mBackgroundModelEnabled;
    
    unsigned int mClusterCount;

    bool mValid;

    std::map<std::string, GMM> mModels;
};

#endif
