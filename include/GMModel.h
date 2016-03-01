#ifndef _GMMODEL_H_
#define _GMMODEL_H_

#include "Common.h"

#include "DynamicVector.h"

#include "Model.h"

/*! \brief A Gaussian Mixture Model using EM-algorithm & MAP-adaptation.
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
    GMModel();

    virtual ~GMModel();
    
    void SetTrainingIterations(unsigned int iterations);
    
    unsigned int GetTrainingIterations() const;

    void SetTrainingThreshold(Real threshold);
    
    Real GetTrainingThreshold() const;

    void Init();

    const std::vector<Cluster>& GetClusters() const;

    /*! \brief Trains the model.
     */
    virtual void Train(const std::vector< DynamicVector<Real> >& samples, unsigned int iterations) override;
    
    /*! \brief Maximum a Posteriori adaptation.
     */
    virtual void Adapt(const std::shared_ptr<Model>& other, const std::vector< DynamicVector<Real> >& samples,
               unsigned int iterations = 2, Real relevanceFactor = 16.0f) override;

    /*! \brief Calculates the normalized log-likelihood value over given samples.
     *
     *  The normalization is done by averaging log-likelihoods by dividing
     *  the sum of sample log-likelihoods by the number of samples.
     *
     *  \param samples Samples of independent observations.
     *  \return Normalized log-likelihood.
     */
    Real GetLogLikelihood(const std::vector< DynamicVector<Real> >& samples) const;
    
    /*! \brief Scores given samples.
     *
     *  \param samples Samples of independent observations.
     *  \return Average score over samples.
     *  \sa GetLogLikelihood()
     */
    virtual Real GetScore(const std::vector< DynamicVector<Real> >& samples) const override;
    
    virtual Real GetLogScore(const std::vector< DynamicVector<Real> >& samples) const override;
    
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
     *  \return Log-likelihood over samples.
     */
    Real E(const std::vector< DynamicVector<Real> >& samples);

    /*! \brief The M-step of the EM-algorithm.
     *
     *  \param samples Samples of independent observations.
     */
    void M(const std::vector< DynamicVector<Real> >& samples);

private:
    /*! \brief Calculates the log-likelihood of the given sample
     *         using given cluster values.
     *
     *  \param values Feature values.
     *  \param cluster A cluster that defines the pdf.
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
