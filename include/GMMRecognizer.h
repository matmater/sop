/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nyk�nen <mnykne@gmail.com>
 */

#ifndef _GMMRECOGNIZER_H_
#define _GMMRECOGNIZER_H_

#include "SpeechData.h"
#include "RecognitionResult.h"
#include "ModelRecognizer.h"

/*! \class GMMRecognizer
 *  \brief Gaussian Mixture Model speaker recognizer.
 */
class GMMRecognizer : public ModelRecognizer
{
public:
    /*! \brief Default constructor.
     */
    GMMRecognizer();

    /*! \brief Virtual destructor.
     */
    virtual ~GMMRecognizer();

protected:
    /*! \brief Create a new Gaussian Mixture Model.
     *
     *  \return A new GMModel instance.
     */
    virtual std::shared_ptr<Model> CreateModel();
};

#endif
