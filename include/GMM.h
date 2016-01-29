#ifndef _GMM_H_
#define _GMM_H_

#include "Common.h"

#include "DynamicVector.h"

/*! \brief A Gaussian Mixture Model using EM-algorithm.
 *
 *  See PDF-file for detailed equations (variable naming may differ).
 *
 *  \warning Can be unstable when data dimensions are too big.
 */
class GMM
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
    /*! \brief Constructor.
     */
    GMM();

    /*! \brief Destructor.
     */
    virtual ~GMM();

    /*! \brief Sets the number of clusters.
     */
    void SetClusterCount(unsigned int clusterCount);
    
    /*! \brief Gets the number of clusters.
     */
    unsigned int GetClusterCount() const;
    
    /*! \brief Returns all clusters.
     */
    const std::vector<Cluster>& GetClusters() const;

    /*! \brief Trains the model.
     */
    void Train(const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Tests the model.
     */
    void Test(const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Calculates a normalized log-likelihood value over given samples.
     *
     *  The normalization is done by averaging log-likelihoods by dividing
     *  the sum of sample log-likelihoods by the number of samples.
     *
     *  \param samples Samples of independent observations.
     *  \return Normalized log-likelihood.
     */
    Real GetLogLikelihood(const std::vector< DynamicVector<Real> >& samples);

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
    Real GetLogLikelihood(const DynamicVector<Real>& values, const Cluster& cluster);

    /*! \brief Precalculates some pdf values for efficiency.
     *
     *  \param cluster A cluster to be modifed.
     */
    void UpdatePDF(Cluster& cluster);

private:
    unsigned mClusterCount;

    Real mEta;

    bool mValid;

    std::vector<Cluster> mClusters;
};

#endif
