#ifndef _MAP_H_
#define _MAP_H_

#include "Common.h"

#include "DynamicVector.h"

/*! \brief MAP algorithm for adapting a speaker model. Based on: ftp://ftp.cs.joensuu.fi/franti/papers/VQMAP-SPL2008.pdf
 */
class MAP
{
public:
    /*! \brief Construct with parameters.
     *
     *  \param clusterCount The number of clusters. Must be power of two!
     *  \param I How many times the algorithm is iterated. Default 2.
     *  \param r Relevance factor, vector count. Does something. Default 12.
     */
    MAP(unsigned int clusterCount = 128, unsigned int I = 2, unsigned int r = 12);

    /*! \brief Virtual destructor.
     */
    virtual ~MAP();
    
    /*! \brief Sets the number of clusters.
     */
    void SetClusterCount(unsigned int clusterCount);
    
    /*! \brief Gets the number of clusters.
     */
    unsigned int GetClusterCount() const;

    /*! \brief Makes codebook out of given samples using MAP algorithm.
     *
     *  \param samples A vector of samples.
     *  \param indices Indices to clusters for each sample.
     *  \param centroids The clusters.
     *  \param sizes The cluster sizes.
     *
     *  \note Given containers will be resized if necessary.
     */
    void Enroll(
        const std::vector< DynamicVector<Real> >& samples,
        std::vector<unsigned int>& indices,
        std::vector< DynamicVector<Real> >& centroids,
        std::vector<unsigned int>& sizes,
        std::vector< DynamicVector<Real> >& ubmcentroids,
        std::vector<unsigned int>& ubmsizes);

private:
    unsigned int mClusterCount;

    unsigned int mI;
    unsigned int mR;
};

#endif
