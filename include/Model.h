/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
 */

#ifndef _MODEL_H_
#define _MODEL_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

/*! \class Model
 *  \brief Abstract speaker model.
 */
class Model
{
public:
    /*! \brief Default constructor.
     */
    Model();
    
    /*! \brief Virtual destructor.
     */
    virtual ~Model();
    
    /*! \brief Get the number of feature dimensions used in the model.
     *
     *  \return The number of feature dimensions.
     */
    virtual unsigned int GetDimensionCount() const = 0;
    
    /*! \brief Set the model order (clusters/codebooks etc.).
     *
     *  \param order The order: 2, 4, 8, 16, 32, ...
     */
    virtual void SetOrder(unsigned int order);
    
    /*! \brief Get the model order (clusters/codebooks etc.).
     *
     *  \return The order: 2, 4, 8, 16, 32, ...
     */
    virtual unsigned int GetOrder() const;
    
    /*! \brief Train the model.
     *
     *  \param samples Train sample data set.
     *  \param iterations Maximum number of training iterations.
     */
    virtual void Train(const std::vector< DynamicVector<Real> >& samples, unsigned int iterations) = 0;
    
    /*! \brief Adapt the model from another model with speech data.
     *
     *  \param other The model to adapt from.
     *  \param sample The training samples.
     *  \param iterations The number of adaptation iterations.
     *  \param relevanceFactor The adaptation relevance factor.
     */
    virtual void Adapt(const std::shared_ptr<Model>& other, const std::vector< DynamicVector<Real> >& samples,
                       unsigned int iterations = 2, Real relevanceFactor = 16.0f) = 0;
    
    /*! \brief Score given samples.
     *
     *  \param samples Samples of independent observations.
     *  \return Score over samples.
     */
    virtual Real GetScore(const std::vector< DynamicVector<Real> >& samples) const = 0;
    
    /*! \brief Log-score given samples.
     *
     *  \param samples Samples of independent observations.
     *  \return Log-Score over samples.
     */
    virtual Real GetLogScore(const std::vector< DynamicVector<Real> >& samples) const = 0;

private:
    unsigned int mOrder;
};

#endif
