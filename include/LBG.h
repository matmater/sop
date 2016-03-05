/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
 */

#ifndef _LBG_H_
#define _LBG_H_

#include "Common.h"

#include "DynamicVector.h"

/*! \brief Linde-Buzo-Gray algorithm for clustering.
 */
class LBG
{
public:
    /*! \brief Construct with parameters.
     *
     *  \param clusterCount The number of clusters. Must be power of two!
     *  \param eta Split constant.
     */
    LBG(unsigned int clusterCount = 128, Real eta = 0.001f);

    /*! \brief Virtual destructor.
     */
    virtual ~LBG();
    
    /*! \brief Set the number of clusters to find.
     *
     *  \param clusterCount The number of clusters to use.
     */
    void SetClusterCount(unsigned int clusterCount);
    
    /*! \brief Get the number of clusters to find.
     *
     *  \return The number of clusters to use.
     */
    unsigned int GetClusterCount() const;

    /*! \brief Clusters given samples.
     *
     *  \param samples A vector of samples.
     *  \param indices Indices to clusters for each sample.
     *  \param centroids The clusters.
     *  \param sizes The cluster sizes.
     *
     *  \note Given containers will be resized if necessary.
     */
    void Cluster(
        const std::vector< DynamicVector<Real> >& samples,
        std::vector<unsigned int>& indices,
        std::vector< DynamicVector<Real> >& centroids,
        std::vector<unsigned int>& sizes);

private:
    /*! \brief Split operation.
     *
     *  Moves vectors apart.
     */
    void Split(DynamicVector<Real>& a, DynamicVector<Real>& b);

private:
    unsigned int mClusterCount;

    Real mEta;
};

#endif
