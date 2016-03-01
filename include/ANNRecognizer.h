#ifndef _ANNRECOGNIZER_H_
#define _ANNRECOGNIZER_H_

#include "Common.h"
#include "Recognizer.h"
#include "ANN.h"
#include "SpeechData.h"
#include "ANNTrainData.h"

class ANNRecognizer : public Recognizer
{
public:
    ANNRecognizer();
    
    virtual ~ANNRecognizer();

    virtual void Clear();
    
    void Train(const SpeechData& data);
    
    void SaveTrainedData(const std::string& path);
    
    void LoadTrainedData(const std::string& path);
    
    void Test(const SpeechData& data, std::map<SpeakerKey, RecognitionResult>& results);

private:
    std::map<unsigned int, SpeakerKey> mNeuronMapping;

    ANNTrainData mTrainData;

    std::vector<Real> mResult;

    ANN mNetwork;
};

#endif