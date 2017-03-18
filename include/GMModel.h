/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _GMMODEL_H_
#define _GMMODEL_H_

#include "Common.h"

#include "DynamicVector.h"

#include "Model.h"

/*! \class GMModel
 *  \brief A Gaussian Mixture Model with EM algorithm & MAP adaptation.
 */
class GMModel : public Model
{
public:
    struct Cluster
    {
        DynamicVector<Real> means; /*!< The means of each dimension. */
        DynamicVector<Real> variances; /*!< The variances of each dimension. */
        Real mixingCoefficient; /*!< The mixing factor / weight of this cluster. */

        // Utility variables do not touch these.

        DynamicVector<Real> meansTmp;

        DynamicVector<Real> variancesTmp;
        DynamicVector<Real> variancesInv;

        Real membershipProbability; /*!< P(k|x_n;phi) */
        Real membershipProbabilitySum; /*!< sum(P(k|x_n;phi)) */

        Real pdfConstant; /*!< A precalculated constant for faster pdf calculations. */
    };

public:
    /*! \brief Default constructor.
     */
    GMModel();

    /*! \brief Virtual destructor.
     */
    virtual ~GMModel();

    /*! \brief Set the maximum number of training iterations.
     *
     *  \param iterations The new number of iterations.
     */
    void SetTrainingIterations(unsigned int iterations);

    /*! \brief Get the maximum number of training iterations.
     *
     *  \return The number of iterations.
     */
    unsigned int GetTrainingIterations() const;

    /*! \brief Set the training threshold used in model training.
     *
     *  \param threshold The new training threshold.
     */
    void SetTrainingThreshold(Real threshold);

    /*! \brief Get the training threshold used in model training.
     *
     *  \return The training threshold.
     */
    Real GetTrainingThreshold() const;

    /*! Initialize the model with default values.
     */
    void Init();

    /*! \brief Get all mixture components.
     *
     *  \return A vector containing all mixture components.
     */
    const std::vector<Cluster>& GetClusters() const;

    /*! \brief Train the model with speech data using EM algorithm.
     *
     *  \param samples Train sample data set.
     *  \param itertions Maximum number of training iterations.
     */
    virtual void Train(const std::vector< DynamicVector<Real> >& samples, unsigned int iterations) override;

    /*! \brief Adapt the model from another model with speech data using MAP adaptation.
     *
     *  \param other The model to adapt from.
     *  \param sample The training samples.
     *  \param iterations The number of MAP iterations.
     *  \param relevanceFactor The MAP relevance factor.
     */
    virtual void Adapt(const std::shared_ptr<Model>& other, const std::vector< DynamicVector<Real> >& samples,
               unsigned int iterations = 2, Real relevanceFactor = 16.0f) override;

    /*! \brief Calculate the normalized log-likelihood value over given samples.
     *
     *  The normalization is done by averaging log-likelihoods by dividing
     *  the sum of sample log-likelihoods by the number of samples.
     *
     *  \param samples Samples of independent observations.
     *
     *  \return Normalized log-likelihood.
     */
    Real GetLogLikelihood(const std::vector< DynamicVector<Real> >& samples) const;

    /*! \brief Score given samples.
     *
     *  \param samples Samples of independent observations.
     *
     *  \return Average score over samples.
     */
    virtual Real GetScore(const std::vector< DynamicVector<Real> >& samples) const override;

    /*! \brief Log-score given samples.
     *
     *  \param samples Samples of independent observations.
     *
     *  \return Average log-score over samples.
     */
    virtual Real GetLogScore(const std::vector< DynamicVector<Real> >& samples) const override;

    /*! \brief Get the number of feature dimensions used in the model.
     *
     *  \return The number of feature dimensions.
     */
    virtual unsigned int GetDimensionCount() const override;

private:
    /*! \brief Initializes cluster variables before the actual EM-algorithm.
     *
     *  \param samples Samples of independent observations.
     */
    void InitClusters(const std::vector< DynamicVector<Real> >& samples);

    /*! \brief The main Expectation-Maximization algorithm.
     *
     *  \param samples Samples of independent observations.
     */
    void EM(const std::vector< DynamicVector<Real> >& samples);

    /*! \brief The E-step of the EM-algorithm.
     *
     *  \param samples Samples of independent observations.
     *
     *  \return Log-likelihood over samples.
     */
    Real E(const std::vector< DynamicVector<Real> >& samples);

    /*! \brief The M-step of the EM-algorithm.
     *
     *  \param samples Samples of independent observations.
     */
    void M(const std::vector< DynamicVector<Real> >& samples);

private:
    /*! \brief Calculate the log-likelihood of the given sample
     *  using given cluster values.
     *
     *  \param values Feature values.
     *  \param cluster A cluster that defines the pdf.
     *
     *  \return Log-likelihood
     */
    Real GetLogLikelihood(const DynamicVector<Real>& values, const Cluster& cluster) const;

    /*! \brief Precalculates constant pdf values for efficiency.
     *
     *  \param cluster A cluster to be modifed.
     */
    void UpdatePDF(Cluster& cluster);

private:
    unsigned int mTrainingIterations;

    Real mEta;

    bool mValid;

    mutable std::vector<Cluster> mClusters;
};

#endif
