/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
 */

#ifndef _ANNRECOGNIZER_H_
#define _ANNRECOGNIZER_H_

#include "Common.h"
#include "Recognizer.h"
#include "ANN.h"
#include "SpeechData.h"
#include "ANNTrainData.h"

/*! \class ANNRecognizer
 *  \brief Artificial Neural Network speaker recognizer.
 *
 *  \note This is currently detached from the main speaker recognition
 *  framework.
 *  \note This is an untested class and more tweaking is needed.
 */
class ANNRecognizer : public Recognizer
{
public:
    /*! \brief Default constructor.
     */
    ANNRecognizer();
    
    /*! \brief Virtual detructor.
     */
    virtual ~ANNRecognizer();
    
    /*! \brief Clear all trained data.
     *
     *  \todo Implement.
     */
    virtual void Clear();
    
    /*! \brief Set speaker speech data.
     *
     *  \param data Speaker speech data for training.
     */
    virtual void SetSpeakerData(std::shared_ptr<SpeechData> data);

    /*! \brief Get speaker speech data.
     *
     *  \return Shared pointer to speaker speech data.
     */
    virtual std::shared_ptr<SpeechData> GetSpeakerData();

    /*! \brief Train the network with given speech data.
     *
     *  \param data The data set used for training.
     */
    virtual void Train() override;
    
    /*! \brief Check if a speaker was recognized.
     *
     *  \param speaker The speaker to be recognized.
     *  \param samples The samples of the speaker.
     *
     *  \return Return true if the speaker was recognized, false otherwise.
     */
    virtual bool IsRecognized(const SpeakerKey& speaker, const std::vector< DynamicVector<Real> >& samples) override;
    
    /*! \brief Verify the claimed speaker.
     *
     *  \param speaker The speaker to be verified.
     *  \param data Speech data set to derive verification scores.
     *
     *  \return A vector containing verification scores of the speaker.
     */
    virtual std::vector<Real> Verify(const SpeakerKey& speaker, const std::shared_ptr<SpeechData>& data) override;
    
private:
    std::map<unsigned int, SpeakerKey> mNeuronMapping;

    std::shared_ptr<SpeechData> mSpeakerData;

    ANNTrainData mTrainData;

    ANN mNetwork;
};

#endif