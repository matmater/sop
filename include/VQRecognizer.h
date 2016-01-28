#ifndef _VQ_H_
#define _VQ_H_

#include "SpeechData.h"

#include "Common.h"

#include "DynamicVector.h"
#include "LBG.h"
#include "Recognizer.h"

/*! \brief A speaker recognizer based on Vector Quantization.
 */
class VQRecognizer : public Recognizer
{
public:
    struct Codebook
    {
        std::vector< DynamicVector<Real> > clusterCentroids;
        std::vector<unsigned int> clusterSizes;
        std::vector<Real> clusterWeights;
    };

public:
    /*! \brief Default constructor.
     */
    VQRecognizer();

    /*! \brief Virtual destructor.
     */
    virtual ~VQRecognizer();

    /*! \brief Enables/disables the background model.
     */
    void SetBackgroundModelEnabled(bool enabled);

    /*! \brief Checks if background model is enabled.
     */
    bool IsBackgroundModelEnabled() const;

    /*! \brief Returns the dimension count based on trained codebooks.
     */
    unsigned int GetDimensionCount();

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
    /*! \brief Trains a codebook.
     */
    void Train(Codebook& codebook, const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Calcualtes the VQ distortion.
     *
     *  \bug Inverse?
     *  \todo Check if inverse.
     */
    Real Distortion(const Codebook& codebook, const std::vector< DynamicVector<Real> >& samples);

private:
    bool mBackgroundModelEnabled;

    std::vector<unsigned int> mClusterSamples;

    std::map<std::string, Codebook> mCodebooks;
};

#endif
