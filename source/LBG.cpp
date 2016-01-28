#include "LBG.h"

LBG::LBG(unsigned int clusterCount, Real eta)
    : mClusterCount(clusterCount), mEta(eta)
{

}

LBG::~LBG()
{

}

void LBG::SetClusterCount(unsigned int clusterCount)
{
    mClusterCount = clusterCount;
}

unsigned int LBG::GetClusterCount() const
{
    return mClusterCount;
}

void LBG::Cluster(
    const std::vector< DynamicVector<Real> >& samples,
    std::vector<unsigned int>& indices,
    std::vector< DynamicVector<Real> >& centroids,
    std::vector<unsigned int>& sizes)
{
    if (indices.size() != samples.size())
    {
        indices.resize(samples.size());
    }
    
    if (centroids.size() != mClusterCount)
    {
        centroids.resize(mClusterCount);
    }

    if (sizes.size() != mClusterCount)
    {
        sizes.resize(mClusterCount);
    }

    // Initialize the feature vectors of the centroids.

    for (auto& centroid : centroids)
    {
        centroid.Resize(samples[0].GetSize(), 0.0f);
    }

    // Create the initial centroid by averaging sample data.

    centroids[0].Assign(0.0f);

    for (const auto& sample : samples)
    {
        centroids[0].Add(sample);
    }

    centroids[0].Multiply(1.0f / static_cast<Real>(samples.size()));

    // Cluster counter.
    unsigned int n = 1;

    // Average distortion.
    Real avgDist = 0.0f;

    for (const auto& sample : samples)
    {
        avgDist += sample.Distance(centroids[0]);
    }

    avgDist /= static_cast<Real>(samples.size() * centroids[0].GetSize());

    //std::cout << avgDist << std::endl;

    do
    {
        //std::cout << "n: " << n << std::endl;

        for (unsigned int c = 0; c < n; ++c)
        {
            Split(centroids[c], centroids[n + c]);
        }

        n *= 2;

        while (true)
        {
            // Find closest centroid for each sample.
            for (unsigned int s = 0; s < samples.size(); ++s)
            {
                Real minDist = std::numeric_limits<Real>::max();

                unsigned int minC = -1;

                for (unsigned int c = 0; c < n; ++c)
                {
                    Real dist = samples[s].Distance(centroids[c]);

                    if (dist < minDist)
                    {
                        minDist = dist;
                        minC = c;
                    }
                }

                indices[s] = minC;
            }

            // Update centroids.
            for (unsigned int c = 0; c < n; ++c)
            {
                centroids[c].Assign(0.0f);

                sizes[c] = 0;
            }

            for (unsigned int s = 0; s < samples.size(); ++s)
            {
                centroids[indices[s]].Add(samples[s]);

                ++sizes[indices[s]];
            }

            for (unsigned int c = 0; c < n; ++c)
            {
                if (sizes[c] > 0)
                {
                    centroids[c].Multiply(1.0f / static_cast<Real>(sizes[c]));
                }
            }

            Real newAvgDist = 0.0f;

            for (unsigned int s = 0; s < samples.size(); ++s)
            {
                newAvgDist += samples[s].Distance(centroids[indices[s]]);
            }

            newAvgDist /= static_cast<Real>(samples.size() * centroids[0].GetSize());

            //std::cout << avgDist << "-" << newAvgDist<< "-" << ((avgDist - newAvgDist) / avgDist) << std::endl;

            if (((avgDist - newAvgDist) / avgDist) > mEta)
            {
                avgDist = newAvgDist;
                continue;
            }

            avgDist = newAvgDist;
            break;
        }

    } while (n < centroids.size());
}

void LBG::Split(DynamicVector<Real>& a, DynamicVector<Real>& b)
{
    Real facA = (1.0f + mEta);
    Real facB = (1.0f - mEta);

    for (unsigned int i = 0; i < a.GetSize(); i++)
    {
        b[i] = a[i] * facB;

        a[i] *= facA;
    }
}
