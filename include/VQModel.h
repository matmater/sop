#ifndef _VQMODEL_H_
#define _VQMODEL_H_

#include "SpeechData.h"

#include "Common.h"

#include "DynamicVector.h"
#include "LBG.h"
#include "Model.h"

/*! \brief A speaker recognizer based on Vector Quantization.
 *
 *  \todo Add comments.
 */
class VQModel : public Model
{
public:
    /*! \brief Default constructor.
     */
    VQModel();

    /*! \brief Virtual destructor.
     */
    virtual ~VQModel();
    
    void Init();

    /*! \brief Weights centroids.
     */
    void Weight(const std::map< std::string, std::shared_ptr<VQModel> >& models);

    /*! \brief Trains the model.
     */
    virtual void Train(const std::vector< DynamicVector<Real> >& samples) override;
    
    /*! \brief Trains the model using MAP adaptation.
     *
     *  MAP algorithm for adapting a speaker model. Based on: ftp://ftp.cs.joensuu.fi/franti/papers/VQMAP-SPL2008.pdf
     */
    virtual void Adapt(const std::shared_ptr<Model>& other, const std::vector< DynamicVector<Real> >& samples,
                       unsigned int iterations = 2, Real relevanceFactor = 16.0f) override;
    
    /*! \brief Returns squared-error distortion measure
     *  divided by the number of samples.
     */
    Real GetDistortion(const std::vector< DynamicVector<Real> >& samples) const;
    
    /*! \brief Returns similarity score using weighting feature.
     *
     * Following: Speaker Discriminative Weighting Method for VQ-based Speaker identification
     * http://www.cs.joensuu.fi/pages/tkinnu/webpage/pdf/DiscriminativeWeightingMethod.pdf
     */
    Real GetWeightedSimilarity(const std::vector< DynamicVector<Real> >& samples) const;
    
    /*! \copydoc Model::GetScore()
     */
    virtual Real GetScore(const std::vector< DynamicVector<Real> >& samples) const override;
    
    /*! \copydoc Model::GetLogScore()
     */
    virtual Real GetLogScore(const std::vector< DynamicVector<Real> >& samples) const override;
    
    /*! \copydoc Model::GetDimensionCount()
     */
    virtual unsigned int GetDimensionCount() const override;

private:
    std::vector< DynamicVector<Real> > mClusterCentroids;
    std::vector<unsigned int> mClusterSizes;
    std::vector<Real> mClusterWeights;
    
    mutable std::vector<unsigned int> mClusterSamples;
};

#endif
