#include "MAP.h"

MAP::MAP(unsigned int clusterCount, unsigned int I, unsigned int r)
    : mClusterCount(clusterCount), mI(I), mR(r)
{

}

MAP::~MAP()
{

}

void MAP::SetClusterCount(unsigned int clusterCount)
{
    mClusterCount = clusterCount;
}

unsigned int MAP::GetClusterCount() const
{
    return mClusterCount;
}

void MAP::Enroll(
    const std::vector< DynamicVector<Real> >& samples,
    std::vector<unsigned int>& indices,
    std::vector< DynamicVector<Real> >& centroids,
    std::vector<unsigned int>& sizes,
    std::vector< DynamicVector<Real> >& ubmcentroids,
    std::vector<unsigned int>& ubmsizes)
{
    if (indices.size() != samples.size())
    {
        //Indices contains the indices of centroids that each sample belongs to
        indices.resize(samples.size());
    }
    
    if (centroids.size() != mClusterCount)
    {
        centroids.resize(mClusterCount);
    }

    if (sizes.size() != mClusterCount)
    {
        //Sizes contains the amount of samples in each centroid
        sizes.resize(mClusterCount);
    }

    // Initialize the feature vectors of the centroids.

    for (auto& centroid : centroids)
    {
        centroid.Resize(samples[0].GetSize(), 0.0f);
    }

    // Set the initial centroids from ubm
    for (int c; c < mClusterCount; c++) {
        centroids[c].Assign(ubmcentroids[c]);
    }

    //Run the algorithm mI times
    for (int i; i<mI; i++) {
        //Find the closest centroid to each sample
        for (int n; n<samples.size(); n++) {
            Real minDist = std::numeric_limits<Real>::max();
            for (int c; c < mClusterCount; c++) {   
                Real dist = samples[n].Distance(centroids[c]);
                if (dist < minDist) {
                    minDist = dist;
                    indices[n] = c;
                }
            }
            
        }

        
        //Set the centroids to the average of the samples in each centroid
        for (unsigned int c = 0; c < mClusterCount; ++c)
        {
            centroids[c].Assign(0.0f);

            sizes[c] = 0;
        }

        for (unsigned int s = 0; s < samples.size(); ++s)
        {
            centroids[indices[s]].Add(samples[s]);

            ++sizes[indices[s]];
        }

        for (unsigned int c = 0; c < mClusterCount; ++c)
        {
            if (sizes[c] > 0)
            {
                centroids[c].Multiply(1.0f / static_cast<Real>(sizes[c]));
            }
        }
        
        
        //Calculate the adapted values
        for (unsigned int c = 0; c < mClusterCount; ++c)
        {
            Real size = static_cast<Real>(sizes[c]);
            Real w = size / (size+static_cast<Real>(mR));
            
            DynamicVector<Real> ubmc;
            ubmc.Assign(ubmcentroids[c]);
            ubmc.Multiply(1.0f-w);            
            
            centroids[c].Multiply(w);
            centroids[c].Add(ubmc);
            
        }
        
    }

}