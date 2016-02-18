#include "VQModel.h"

VQModel::VQModel()
{

}

VQModel::~VQModel()
{

}

void VQModel::ResetWeights()
{
    for (Real& weight : mClusterWeights)
    {
        weight = 1.0f;
    }
}

void VQModel::Init()
{
    if (mClusterCentroids.size() != GetOrder())
    {
        mClusterCentroids.resize(GetOrder());
    }

    if (mClusterSizes.size() != GetOrder())
    {
        mClusterSizes.resize(GetOrder());
    }
    
    if (mClusterWeights.size() != GetOrder())
    {
        mClusterWeights.resize(GetOrder());
    }
}

void VQModel::Train(const std::vector< DynamicVector<Real> >& samples, unsigned int iterations)
{
    LBG lbg(GetOrder());

    mClusterWeights.resize(GetOrder());
    
    ResetWeights();

    std::vector<unsigned int> indices;

    lbg.Cluster(samples, indices, mClusterCentroids, mClusterSizes);
}

void VQModel::Adapt(const std::shared_ptr<Model>& other, const std::vector< DynamicVector<Real> >& samples,
    unsigned int iterations, Real relevanceFactor)
{
    const VQModel* model = dynamic_cast<VQModel*>(other.get());

    if (model == nullptr)
    {
        std::cout << "Not VQModel." << std::endl;
        return;
    }

    SetOrder(model->GetOrder());
    Init();

    std::vector<unsigned int> indices(samples.size());

    // Initialize the feature vectors of the centroids.

    for (unsigned int c = 0; c < GetOrder(); c++)
    {
        mClusterCentroids[c] = model->mClusterCentroids[c];
    }

    // Do the iterations.
    for (unsigned int i = 0; i < iterations; i++)
    {
        //Find the closest centroid to each sample
        for (unsigned int n = 0; n < samples.size(); n++)
        {
            Real minDist = std::numeric_limits<Real>::max();

            for (unsigned int c = 0; c < GetOrder(); c++)
            {
                Real dist = samples[n].Distance(mClusterCentroids[c]);

                if (dist < minDist)
                {
                    minDist = dist;
                    indices[n] = c;
                }
            }
        }

        //Set the centroids to the average of the samples in each centroid
        for (unsigned int c = 0; c < GetOrder(); ++c)
        {
            mClusterCentroids[c].Assign(0.0f);

            mClusterSizes[c] = 0;
        }

        for (unsigned int s = 0; s < samples.size(); ++s)
        {
            mClusterCentroids[indices[s]].Add(samples[s]);

            ++mClusterSizes[indices[s]];
        }

        for (unsigned int c = 0; c < GetOrder(); ++c)
        {
            if (mClusterSizes[c] > 0)
            {
                mClusterCentroids[c].Multiply(1.0f / static_cast<Real>(mClusterSizes[c]));
            }
        }

        //Calculate the adapted values
        for (unsigned int c = 0; c < GetOrder(); ++c)
        {
            Real size = static_cast<Real>(mClusterSizes[c]);
            Real w = size / (size + static_cast<Real>(relevanceFactor));

            DynamicVector<Real> ubmc = model->mClusterCentroids[c];
            ubmc.Multiply(1.0f - w);

            mClusterCentroids[c].Multiply(w);
            mClusterCentroids[c].Add(ubmc);
        }
    }
}

void VQModel::Weight(const std::map< SpeakerKey, std::shared_ptr<Model> >& models)
{
    // Following Speaker Discriminative Weighting Method for VQ-based Speaker identification
    // http://www.cs.joensuu.fi/pages/tkinnu/webpage/pdf/DiscriminativeWeightingMethod.pdf

    for (unsigned int i = 0; i < mClusterCentroids.size(); i++)
    {
        if (mClusterSizes[i] == 0)
        {
            continue;
        }

        Real sum = 0.0f;

        for (auto& b : models)
        {
            if (b.second.get() == this)
            {
                //DEBUG_TRACE("Same model.");

                continue;
            }

            Real dmin = std::numeric_limits<Real>::max();

            const VQModel* other = dynamic_cast<VQModel*>(b.second.get());

            for (unsigned int j = 0; j < other->mClusterCentroids.size(); j++)
            {
                if (other->mClusterSizes[j] == 0)
                {
                    continue;
                }

                Real dist = other->mClusterCentroids[j].Distance(mClusterCentroids[i]);

                if (dist < dmin)
                {
                    dmin = dist;
                }
            }

            sum += 1.0f / dmin;
        }

        mClusterWeights[i] = 1.0f / sum;
    }
}

Real VQModel::GetDistortion(const std::vector< DynamicVector<Real> >& samples) const
{
    if (mClusterSamples.size() < samples.size())
    {
        mClusterSamples.resize(samples.size());
    }

    auto& centroids = mClusterCentroids;
    auto& sizes = mClusterSizes;
    auto& weights = mClusterWeights;

    Real dist = 0.0f;

    for (unsigned int s = 0; s < samples.size(); ++s)
    {
        auto& sample = samples[s];

        Real minDist = std::numeric_limits<Real>::max();
        unsigned int minC = -1;

        for (unsigned int c = 0; c < centroids.size(); ++c)
        {
            if (sizes[c] == 0)
            {
                continue;
            }

            dist = sample.Distance(centroids[c]);

            if (dist < minDist)
            {
                minDist = dist;
                minC = c;
            }
        }

        mClusterSamples[s] = minC;
    }

    Real distortion = 0.0f;

    for (unsigned int s = 0; s < samples.size(); ++s)
    {
        distortion += samples[s].Distance(centroids[mClusterSamples[s]]);
    }
    
    return distortion;
}

Real VQModel::GetWeightedSimilarity(const std::vector< DynamicVector<Real> >& samples) const
{
    if (mClusterSamples.size() < samples.size())
    {
        mClusterSamples.resize(samples.size());
    }

    auto& centroids = mClusterCentroids;
    auto& sizes = mClusterSizes;
    auto& weights = mClusterWeights;

    Real dist = 0.0f;

    for (unsigned int s = 0; s < samples.size(); ++s)
    {
        auto& sample = samples[s];

        Real minDist = std::numeric_limits<Real>::max();
        unsigned int minC = -1;

        for (unsigned int c = 0; c < centroids.size(); ++c)
        {
            if (sizes[c] == 0)
            {
                continue;
            }

            dist = sample.Distance(centroids[c]);

            if (dist < minDist)
            {
                minDist = dist;
                minC = c;
            }
        }

        mClusterSamples[s] = minC;
    }

    Real distortion = 0.0f;

    for (unsigned int s = 0; s < samples.size(); ++s)
    {
        distortion += weights[mClusterSamples[s]]
            / samples[s].Distance(centroids[mClusterSamples[s]]);
    }

    return distortion / static_cast<Real>(samples.size());
}

Real VQModel::GetScore(const std::vector< DynamicVector<Real> >& samples) const
{
    return GetWeightedSimilarity(samples);
}

Real VQModel::GetLogScore(const std::vector< DynamicVector<Real> >& samples) const
{
    return std::log(GetWeightedSimilarity(samples));
}

unsigned int VQModel::GetDimensionCount() const
{
    if (mClusterCentroids.size() == 0)
        return 0;

    return mClusterCentroids.begin()->GetSize();
}