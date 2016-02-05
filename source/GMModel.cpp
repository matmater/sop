#include "GMModel.h"
#include "LBG.h"

GMModel::GMModel()
: mEta(0.001f)
{

}

GMModel::~GMModel()
{

}

void GMModel::Init()
{
    if (mClusters.size() != GetOrder())
    {
        mClusters.resize(GetOrder());
    }
}

void GMModel::Train(const std::vector< DynamicVector<Real> >& samples)
{
    Init();

    InitClusters(samples);

    EM(samples);
}

void GMModel::Adapt(const std::shared_ptr<Model>& other, const std::vector< DynamicVector<Real> >& samples,
                    unsigned int iterations, Real relevanceFactor)
{
    const GMModel* model = dynamic_cast<GMModel*>(other.get());

    if (model == nullptr)
    {
        std::cout << "Not GMModel." << std::endl;
        return;
    }

    SetOrder(other->GetOrder());
    Init();

    for (unsigned int c = 0; c < GetOrder(); c++)
    {
        mClusters[c].means = model->mClusters[c].means;
        mClusters[c].variances = model->mClusters[c].variances;
        mClusters[c].mixingCoefficient = model->mClusters[c].mixingCoefficient;

        mClusters[c].meansTmp = model->mClusters[c].meansTmp;
        mClusters[c].variancesTmp = model->mClusters[c].variancesTmp;
        mClusters[c].variancesInv = model->mClusters[c].variancesInv;
    }

    for (auto& cluster : mClusters)
    {
        UpdatePDF(cluster);
    }

    // TODO remove this garbage if necessary.
    Real logLikelihood = 0.0f;
    Real newLogLikelihood = 0.0f;

    // Not sure about this iteration thing?
    for (unsigned int e = 0; e < iterations; ++e)
    {
        //std::cout << "Iteration:" << e << std::endl;

        for (auto& cluster : mClusters)
        {
            cluster.membershipProbabilitySum = 0.0f;
            cluster.meansTmp.Assign(0.0f);
        }

        Real newLogLikelihood = 0.0f;

        for (const auto& sample : samples)
        {
            // Using LSE for numerical stability.
            Real probMax = std::numeric_limits<Real>::min();
            Real probSumExp = 0.0f;
            Real probLogSumExp = 0.0f;

            for (auto& cluster : mClusters)
            {
                cluster.membershipProbability = GetLogLikelihood(sample, cluster);

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
                }
            }

            newLogLikelihood += probLogSumExp;
        }
        
        for (auto& cluster : mClusters)
        {
            Real n = cluster.membershipProbabilitySum;
            
            Real adaptionCoeff = n / (n + relevanceFactor);

            for (unsigned int d = 0; d < mClusters[0].means.GetSize(); ++d)
            {
                cluster.means[d] = adaptionCoeff * (cluster.meansTmp[d] / n) + (1.0f - adaptionCoeff) * cluster.means[d];
            }

            UpdatePDF(cluster);
        }

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

Real GMModel::GetLogLikelihood(const std::vector< DynamicVector<Real> >& samples) const
{
    if (samples.size() == 0)
    {
        return 0.0f;
    }

    Real result = 0.0f;
    Real invN = 1.0f / static_cast<Real>(samples.size());

    for (const auto& sample : samples)
    {
        // Using LSE for numerical stability.
        Real probMax = std::numeric_limits<Real>::min();
        Real probSumExp = 0.0f;
        Real probLogSumExp = 0.0f;

        for (auto& cluster : mClusters)
        {
            // TODO get rid of this assignment. For now this is "ok" though as long
            // as this function is called outside.
            cluster.membershipProbability = GetLogLikelihood(sample, cluster);

            if (cluster.membershipProbability > probMax)
                probMax = cluster.membershipProbability;
        }

        for (const auto& cluster : mClusters)
            probSumExp += std::exp(cluster.membershipProbability - probMax);

        probLogSumExp = probMax + std::log(probSumExp);

        result += probLogSumExp * invN;
    }

    return result;
}

Real GMModel::GetScore(const std::vector< DynamicVector<Real> >& samples) const
{
    return std::exp(GetLogScore(samples));
}

Real GMModel::GetLogScore(const std::vector< DynamicVector<Real> >& samples) const
{
    return GetLogLikelihood(samples);
}

unsigned int GMModel::GetDimensionCount() const
{
    if (mClusters.size() == 0)
    {
        return 0;
    }

    return mClusters.begin()->means.GetSize();
}

void GMModel::InitClusters(const std::vector< DynamicVector<Real> >& samples)
{
    // Create the initial centroid by averaging sample data.

    for (auto& cluster : mClusters)
    {
        cluster.means.Resize(samples[0].GetSize());
        cluster.variances.Resize(samples[0].GetSize());
        cluster.meansTmp.Resize(samples[0].GetSize());
        cluster.variancesTmp.Resize(samples[0].GetSize());
        cluster.variancesInv.Resize(samples[0].GetSize());
    }

    std::vector<unsigned int> indices;
    std::vector< DynamicVector<Real> > centroids;
    std::vector<unsigned int> sizes;

    LBG lbg(GetOrder());
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

void GMModel::EM(const std::vector< DynamicVector<Real> >& samples)
{
    for (auto& cluster : mClusters)
    {
        UpdatePDF(cluster);
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

Real GMModel::E(const std::vector< DynamicVector<Real> >& samples)
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
            cluster.membershipProbability = GetLogLikelihood(sample, cluster);

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

void GMModel::M(const std::vector< DynamicVector<Real> >& samples)
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

        UpdatePDF(cluster);
    }
}

Real GMModel::GetLogLikelihood(const DynamicVector<Real>& values, const Cluster& cluster) const
{
    Real v = 0.0f;

    for (unsigned int d = 0; d < mClusters[0].means.GetSize(); ++d)
    {
        Real tmp = values[d] - cluster.means[d];
        v += tmp * tmp * cluster.variancesInv[d];
    }

    return (cluster.pdfConstant - 0.5f * v) + std::log(cluster.mixingCoefficient);
}

void GMModel::UpdatePDF(Cluster& cluster)
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
