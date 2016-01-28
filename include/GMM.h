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

    /*! \brief Returns all clusters.
     */
    std::vector<Cluster>& GetClusters();

    /*! \brief Returns all clusters.
     */
    const std::vector<Cluster>& GetClusters() const;

    /*! \brief Destructor.
     */
    virtual ~GMM();

    /*! \brief Trains the model.
     */
    void Train(const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Tests the model.
     */
    void Test(const std::vector< DynamicVector<Real> >& samples);

    /*! \brief Calculates the sum of probability density function
     *         values over given samples.
     *
     *  \param samples Samples of independent observations.
     *  \return The sum of probability density function values over given samples.
     *          See LogClusterGaussianPdf().
     *  \note The returned value is in linear domain (not log domain).
     */
    Real GetValue(const std::vector< DynamicVector<Real> >& samples);

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
    /*! \brief Calculates the log(pdf(values)) of the given cluster.
     *
     *  \param values Feature values.
     *  \param cluster A cluster that defines the pdf.
     *  \return log(pdf(values))
     */
    Real LogClusterGaussianPdf(const DynamicVector<Real>& values, const Cluster& cluster);

    /*! \brief Precalculates some pdf values for efficiency.
     *
     *  \param cluster A cluster to be modifed.
     */
    void UpdateConstants(Cluster& cluster);

private:
    Real mEta;

    bool mValid;

    std::vector<Cluster> mClusters;
};

#endif
