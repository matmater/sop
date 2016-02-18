#ifndef _MODEL_H_
#define _MODEL_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

/*! \todo Add/fix comments.
 */
class Model
{
public:
    Model();

    virtual ~Model();

    virtual unsigned int GetDimensionCount() const = 0;

    virtual void SetOrder(unsigned int order);

    virtual unsigned int GetOrder() const;
    
    virtual void Train(const std::vector< DynamicVector<Real> >& samples, unsigned int iterations) = 0;
    
    virtual void Adapt(const std::shared_ptr<Model>& other, const std::vector< DynamicVector<Real> >& samples,
                       unsigned int iterations = 2, Real relevanceFactor = 16.0f) = 0;
    
    virtual Real GetScore(const std::vector< DynamicVector<Real> >& samples) const = 0;

    virtual Real GetLogScore(const std::vector< DynamicVector<Real> >& samples) const = 0;

private:
    unsigned int mOrder;
};

#endif
