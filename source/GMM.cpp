#include "GMM.h"
#include "LBG.h"

GMM::GMM()
{
    mEta = 0.001f;
    mValid = false;
}

GMM::~GMM()
{

}

std::vector<GMM::Cluster>& GMM::GetClusters()
{
    return mClusters;
}

const std::vector<GMM::Cluster>& GMM::GetClusters() const
{
    return mClusters;
}

void GMM::Train(const std::vector< DynamicVector<Real> >& samples)
{
    InitClusters(samples);

    EM(samples);
}

Real GMM::GetValue(const std::vector< DynamicVector<Real> >& samples)
{
    Real prob = 0.0;

    for (const auto& sample : samples)
    {
        for (auto& cluster : mClusters)
        {
            prob += std::exp(LogClusterGaussianPdf(sample, cluster));
        }
    }

    return prob;
}

void GMM::InitClusters(const std::vector< DynamicVector<Real> >& samples)
{
    unsigned int clusterCount = 128;

    // Create the initial centroid by averaging sample data.

    mClusters.clear();
    mClusters.resize(clusterCount);

    for (auto& cluster : mClusters)
    {
        cluster.means.Resize(samples[0].GetSize());
        cluster.variances.Resize(samples[0].GetSize());
        cluster.meansTmp.Resize(samples[0].GetSize());
        cluster.variancesTmp.Resize(samples[0].GetSize());
        cluster.variancesInv.Resize(samples[0].GetSize());
    }

    std::vector<unsigned int> indices;
    indices.resize(samples.size());

    std::vector< DynamicVector<Real> > centroids;
    centroids.resize(clusterCount);

    std::vector<unsigned int> sizes;
    sizes.resize(clusterCount);

    LBG lbg;
    lbg.Cluster(samples, indices, centroids, sizes);

    for (unsigned int c = 0; c < centroids.size(); c++)
    {
        mClusters[c].means.Assign(centroids[c]);
    }

    Real tot = 0.0f;

    for (auto it = mClusters.begin(); it != mClusters.end(); it++)
    {
        //std::cout << "Cluster :" << sizes[it - mClusters.begin()] << std::endl;

        tot += sizes[it - mClusters.begin()];
    }

    for (unsigned int c = 0; c < mClusters.size(); ++c)
    {
        mClusters[c].mixingCoefficient = sizes[c] / tot;
    }

    for (auto& cluster : mClusters)
    {
        for (unsigned int d = 0; d < mClusters[0].means.GetSize(); ++d)
        {
            for (const auto& sample : samples)
            {
                cluster.variances[d] += (cluster.means[d] - sample[d]) * (cluster.means[d] - sample[d]) / samples.size();
            }
        }
    }
}

void GMM::EM(const std::vector< DynamicVector<Real> >& samples)
{
    for (auto& cluster : mClusters)
    {
        UpdateConstants(cluster);
    }

    Real logLikelihood = 0.0f;
    Real newLogLikelihood = 0.0f;

    for (unsigned int e = 0; e < 200; ++e)
    {
        //std::cout << "Iteration:" << e << std::endl;

        for (auto& cluster : mClusters)
        {
            cluster.membershipProbabilitySum = 0.0f;

            cluster.meansTmp.Assign(0.0f);
            cluster.variancesTmp.Assign(0.0f);
        }

        newLogLikelihood = E(samples);

        M(samples);

        //std::cout << std::abs(logLikelihood - newLogLikelihood) << std::endl;

        if (std::abs(logLikelihood - newLogLikelihood) < mEta)
        {
            break;
        }

        logLikelihood = newLogLikelihood;

        std::cout << ".";
    }

    std::cout << std::endl;
}

Real GMM::E(const std::vector< DynamicVector<Real> >& samples)
{
    Real newLogLikelihood = 0.0f;

    for (const auto& sample : samples)
    {
        // Using LSE for numerical stability.
        Real probMax = std::numeric_limits<Real>::min();
        Real probSumExp = 0.0f;
        Real probLogSumExp = 0.0f;

        for (auto& cluster : mClusters)
        {
            cluster.membershipProbability = LogClusterGaussianPdf(sample, cluster);

            if (cluster.membershipProbability > probMax)
                probMax = cluster.membershipProbability;
        }

        for (const auto& cluster : mClusters)
            probSumExp += std::exp(cluster.membershipProbability - probMax);

        probLogSumExp = probMax + std::log(probSumExp);

        for (auto& cluster : mClusters)
        {
            // Calculate the final membership probability.
            cluster.membershipProbability = std::exp(cluster.membershipProbability - probLogSumExp);

            // Calculate the final sum of membership probabilities.
            cluster.membershipProbabilitySum += cluster.membershipProbability;

            //std::cout << "mp:" << cluster.membershipProbability << std::endl;
            for (unsigned int d = 0; d < cluster.means.GetSize(); ++d)
            {
                // Mean sum(p*x)
                cluster.meansTmp[d] += sample[d] * cluster.membershipProbability;

                // Variance sum(p*x^2) (- mu^2)
                cluster.variancesTmp[d] += sample[d] * sample[d] * cluster.membershipProbability;
            }
        }

        newLogLikelihood += probLogSumExp;
    }

    return newLogLikelihood;
}

void GMM::M(const std::vector< DynamicVector<Real> >& samples)
{
    for (auto& cluster : mClusters)
    {
        Real invMembershipProbabilitySum = 1.0f / cluster.membershipProbabilitySum;

        for (unsigned int d = 0; d < mClusters[0].means.GetSize(); ++d)
        {
            cluster.means[d] = cluster.meansTmp[d] * invMembershipProbabilitySum;
            cluster.variances[d] = cluster.variancesTmp[d] * invMembershipProbabilitySum - cluster.means[d] * cluster.means[d];

            // Limit variance value.
            // When multiplying many numbers with values < 1.0 it is likely that
            // the value goes near zero and it causes instability in the implementation.

            if (cluster.variances[d] < 1.0e-5f)
            {
                cluster.variances[d] = 1.0e-5f;
            }
        }

        cluster.mixingCoefficient = cluster.membershipProbabilitySum / static_cast<Real>(samples.size());

        UpdateConstants(cluster);
    }
}

Real GMM::LogClusterGaussianPdf(const DynamicVector<Real>& values, const Cluster& cluster)
{
    Real v = 0.0f;

    for (unsigned int d = 0; d < mClusters[0].means.GetSize(); ++d)
    {
        Real tmp = values[d] - cluster.means[d];
        v += tmp * tmp * cluster.variancesInv[d];
    }

    return (cluster.pdfConstant - 0.5f * v) + std::log(cluster.mixingCoefficient);
}

void GMM::UpdateConstants(Cluster& cluster)
{
    for (unsigned int d = 0; d < mClusters[0].means.GetSize(); ++d)
    {
        cluster.variancesInv[d] = 1.0f / cluster.variances[d];
    }

    Real constant = 1.0;

    // Determinant
    for (unsigned int d = 0; d < mClusters[0].means.GetSize(); ++d)
    {
        constant *= cluster.variances[d];
    }

    constant = std::log(constant);
    constant += static_cast<Real>(mClusters[0].means.GetSize())* std::log(2.0f * PI_F);

    cluster.pdfConstant = -0.5f * constant;
}
