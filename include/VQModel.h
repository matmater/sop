/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _VQMODEL_H_
#define _VQMODEL_H_

#include "SpeechData.h"

#include "Common.h"

#include "DynamicVector.h"
#include "LBG.h"
#include "Model.h"

/*! \brief Speaker recognizer based on Vector Quantization using LBG
 *  and MAP algorithms.
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

    /*! \brief Reset all weights to 1.
     */
    void ResetWeights();

    /*! \brief Initialize the internal structure of the model.
     */
    void Init();

    /*! \brief Weight centroids.
     *
     *  \param models Models involved in weighting.
     */
    void Weight(const std::map< SpeakerKey, std::shared_ptr<Model> >& models);

    /*! \brief Train the model.
     *
     *  \param samples Train sample data set.
     *  \param itertions Maximum number of training iterations.
     */
    virtual void Train(const std::vector< DynamicVector<Real> >& samples,
        unsigned int iterations) override;

    /*! \brief Train the model using MAP adaptation.
     *
     *  MAP algorithm for adapting a speaker model. Based on:
     *  ftp://ftp.cs.joensuu.fi/franti/papers/VQMAP-SPL2008.pdf
     *
     *  \param other The model to adapt from.
     *  \param sample The training samples.
     *  \param iterations The number of MAP iterations.
     *  \param relevanceFactor The MAP relevance factor.
     */
    virtual void Adapt(const std::shared_ptr<Model>& other,
        const std::vector< DynamicVector<Real> >& samples,
        unsigned int iterations = 2, Real relevanceFactor = 12.0f) override;

    /*! \brief Return squared-error distortion measure
     *  divided by the number of samples.
     *
     *  \param samples Samples of independent observations.
     *
     *  \return Squared-error distortion measure over the samples.
     */
    Real GetDistortion(const std::vector< DynamicVector<Real> >& samples) const;

    /*! \brief Return similarity score using weighting feature.
     *
     * Following: Speaker Discriminative Weighting Method for VQ-based Speaker identification
     * http://www.cs.joensuu.fi/pages/tkinnu/webpage/pdf/DiscriminativeWeightingMethod.pdf
     *
     *  \param samples Samples of independent observations.
     *
     *  \return Weighted similarity measure over the samples.
     */
    Real GetWeightedSimilarity(
        const std::vector< DynamicVector<Real> >& samples) const;

    /*! \brief Score given samples.
     *
     *  \param samples Samples of independent observations.
     *
     *  \return Average score over samples.
     */
    virtual Real GetScore(
        const std::vector< DynamicVector<Real> >& samples) const override;

    /*! \brief Log-score given samples.
     *
     *  \param samples Samples of independent observations.
     *
     *  \return Average score over samples.
     */
    virtual Real GetLogScore(
        const std::vector< DynamicVector<Real> >& samples) const override;

    /*! \brief Get the number of feature dimensions used in the model.
     *
     *  \return The number of feature dimensions.
     */
    virtual unsigned int GetDimensionCount() const override;

private:
    std::vector< DynamicVector<Real> > mClusterCentroids;
    std::vector<unsigned int> mClusterSizes;
    std::vector<Real> mClusterWeights;

    mutable std::vector<unsigned int> mClusterSamples;
};

#endif
