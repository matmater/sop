#include "Common.h"
#include "Utilities.h"
#include <chrono>

#include "GMMRecognizer.h"
#include "VQRecognizer.h"


template <typename T> 
std::string toString(const T& n)
{
    std::ostringstream s;
    s << n;
    return s.str();
}

void LoadTextSamples(const std::shared_ptr<SpeechData>& data, unsigned int sf, unsigned int gf, unsigned int sl, unsigned int gl, bool train)
{
    data->Clear();
    for (unsigned int i = 0; i < gf; i++)
    {
        std::string file = "samples_" + toString(sf+i) + ".txt";
        std::cout << file << std::endl;
        data->Load(file, sl, gl, train);
    }
    data->Validate();
    data->Normalize();
}

void Evaluate(std::map<std::string, RecognitionResult>& results, std::chrono::duration<double> testDur, std::chrono::duration<double> loadDur)
{
    int correctlyRecognized = 0;
    int correctlyRejected = 0;
    int incorrectlyRecognized = 0;
    int incorrectlyRejected = 0;

    for (auto& result : results)
    {
        const RecognitionResult& rr = result.second;

        if (rr.GetKnown())
        {
            // Known speaker
            if (Utilities::IsSameSpeaker(result.first, rr.GetSpeaker()))
            {
                correctlyRecognized++;
            }

            else if (rr.GetSpeaker() == "")
            {
                incorrectlyRejected++;
            }
            else
            {
                incorrectlyRecognized++;
            }
        }

        else
        {
            // Unknown speaker
            if (rr.GetSpeaker() == "")
            {
                correctlyRejected++;
            }

            else
            {
                incorrectlyRecognized++;
            }
        }
    }

    DEBUG_TRACE(
        "correctlyRecognized"
        << " "
        << "correctlyRejected"
        << " "
        << "incorrectlyRecognized"
        << " "
        << "incorrectlyRejected");

    DEBUG_TRACE(
        correctlyRecognized
        << " "
        << correctlyRejected
        << " "
        << incorrectlyRecognized
        << " "
        << incorrectlyRejected);

    std::ofstream resultss;
    resultss.open("results.txt", std::ios::app);
    resultss << correctlyRecognized << " " << correctlyRejected << " " << incorrectlyRecognized << " " << incorrectlyRejected << " "
    << loadDur.count() << " " << testDur.count() << std::endl;
    resultss.close();
}

int main(int argc, char** argv)
{
    /*
    {
        std::cout << "Test" << std::endl;
        GMMRecognizer recognizer;
        //GMMRecognizer recognizer;
        //ANNRecognizer recognizer;

        auto trainData = std::make_shared<SpeechData>();
        trainData->Load("train.txt");
        trainData->Normalize();
        recognizer.SetBackgroundModelEnabled(true);
        recognizer.SetOrder(128);
        //recognizer.SetWeightingEnabled(true);
        recognizer.SetScoreNormalizationType(NormalizationType::ZERO_TEST);
        recognizer.SetImpostorSpeakerData(trainData);
        std::map<std::string, RecognitionResult> result;
        auto testData = std::make_shared<SpeechData>();
        auto start = std::chrono::system_clock::now();
        testData->Load("test.txt");
        testData->Normalize();
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> loadDuration = end - start;
        recognizer.Train(trainData);
        start = std::chrono::system_clock::now();
        recognizer.Test(testData, result);
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> testDuration = end - start;
        // Result will be analyzed here.
        Evaluate(result, loadDuration, testDuration);
    }
    */

    unsigned int sf = 240;   // startfile, speaker to start with
    unsigned int gf = 2;    // getfiles, number of files to get
    unsigned int sl = 1;    // startline, line to start at
    unsigned int gl = 5;    // getlines, number of lines to get
    unsigned int trainingCycles = 5;
    unsigned int incorrectClaimed = 5;
    unsigned int correctClaimed = 5;

    
    VQRecognizer recognizer;
    recognizer.SetBackgroundModelEnabled(false);
    recognizer.SetOrder(128);
    recognizer.SetWeightingEnabled(false);
    auto trainData = std::make_shared<SpeechData>();
    auto testData = std::make_shared<SpeechData>();
    auto testData2 = std::make_shared<SpeechData>();
    std::string claimedSpeaker;
    std::ofstream vresults;
    vresults.open("verificationresults.txt", std::ios::app);
    std::vector<Real> verificationResults;
    
    for (unsigned int i = 0; i < trainingCycles ; i++)
    {
        std::cout << i+1 << "/" << trainingCycles << std::endl;
        LoadTextSamples(trainData, sf+i, gf, sl, gl, true);
        recognizer.Train(trainData);
        LoadTextSamples(testData2, sf+i+gf, incorrectClaimed, sl, gl, false);          
        for (unsigned int j = 0; j < gf; j++)
        {
            LoadTextSamples(testData, sf+i+j, 1, sl+gl, correctClaimed, false);          
            claimedSpeaker = toString(sf+i+j);
            verificationResults = recognizer.Verify(claimedSpeaker, testData);
            for (auto& entry : verificationResults)
            {
                vresults << entry << " ";
            }
            vresults << std::endl;
            verificationResults = recognizer.Verify(claimedSpeaker, testData2);
            for (auto& entry : verificationResults)
            {
                vresults << entry << " ";
            }
            vresults << std::endl;
        }
        sf += gf;
    }
   

/*

    trainData->Load("trainubm.txt");
    trainData->Normalize();
    auto testData = std::make_shared<SpeechData>();
    std::string claimedSpeaker = "225";
    recognizer.Train(trainData);
    std::ofstream recresults;
    recresults.open("recresults.txt", std::ios::app);
    std::vector<Real> verification_results;
    std::string testFile;


    testFile = "test2.txt";
    claimedSpeaker = "225";
    testData->Load(testFile);
    testData->Normalize();
    verification_results = recognizer.Verify(claimedSpeaker, testData);
    for (auto& entry : verification_results)
    {
        recresults << entry << " ";
    }

    recresults << std::endl;

    testFile = "test3.txt";
    testData->Load(testFile);
    testData->Normalize();
    verification_results = recognizer.Verify(claimedSpeaker, testData);
    for (auto& entry : verification_results)
    {
        recresults << entry << " ";
    }

    recresults.close();

*/

}
