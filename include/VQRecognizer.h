/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
 */

#ifndef _VQRECOGNIZER_H_
#define _VQRECOGNIZER_H_

#include "SpeechData.h"

#include "Common.h"

#include "ModelRecognizer.h"
#include "DynamicVector.h"
#include "LBG.h"
#include "VQModel.h"

/*! \brief A speaker recognizer based on Vector Quantization.
 */
class VQRecognizer : public ModelRecognizer
{
public:
    /*! \brief Default constructor.
     */
    VQRecognizer();

    /*! \brief Virtual destructor.
     */
    virtual ~VQRecognizer();

    /*! \brief Enable or disable codevector weighting.
     *
     *  \param enabled True to enable weighting, false to disable.
     */
    void SetWeightingEnabled(bool enabled);
    
    /*! \brief Check if codevector weighting is enabled.
     *
     *  \return True if weighting is enabled, false otherwise.
     */
    bool IsWeightingEnabled() const;

protected:
    /*! \brief Prepare models after training before scoring.
     */
    virtual void PrepareModels();

protected:
    /*! \brief Create a new VQ codebook model.
     *
     *  \return A new VQModel instance.
     */
    virtual std::shared_ptr<Model> CreateModel();

private:
    bool mWeightingEnabled;
};

#endif
