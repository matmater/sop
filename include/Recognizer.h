#ifndef _RECOGNIZER_H_
#define _RECOGNIZER_H_

#include "Common.h"

#include "RecognitionResult.h"

#include "SpeechData.h"

/*! \brief The main structure of a speaker recognizer.
 */
class Recognizer
{
public:
    /*! \brief Virtual destructor.
     */
    virtual ~Recognizer() { }

    /*! \brief Clears all data.
     */
    virtual void Clear() = 0;

    /*! \brief Trains the recognizer with given speech data.
     */
    virtual void Train(const SpeechData& data) = 0;

    /*! \brief Loads trained data.
     */
    virtual void LoadTrainedData(const std::string& path) = 0;

    /*! \brief Saves trained data.
     */
    virtual void SaveTrainedData(const std::string& path) = 0;

    /*! \brief Tests samples on the trained recognizer.
     */
    virtual void Test(const SpeechData& data, std::map<std::string, RecognitionResult>& results) = 0;
};

#endif
