#ifndef _RECOGNIZER_H_
#define _RECOGNIZER_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

/*! \brief The main structure of a speaker recognizer.
 *
 *  \todo Add/fix comments.
 */
class Recognizer
{
public:
    /*! \brief Virtual destructor.
     */
    virtual ~Recognizer() { }

    /*! \brief Trains the recognizer with given speech data.
     */
    virtual void Train(const std::shared_ptr<SpeechData>& data) = 0;

    /*! \brief Loads trained data.
     */
    virtual void LoadTrainedData(const std::string& path) = 0;

    /*! \brief Saves trained data.
     */
    virtual void SaveTrainedData(const std::string& path) = 0;

    /*! \brief Tests samples on the trained recognizer.
     */
    virtual void Test(const std::shared_ptr<SpeechData>& data, std::map<std::string, RecognitionResult>& results) = 0;
};

#endif
