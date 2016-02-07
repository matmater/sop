#include "Common.h"
#include "Utilities.h"
#include <chrono>

#include "GMMRecognizer.h"
#include "VQRecognizer.h"

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

    VQRecognizer recognizer;
    recognizer.SetBackgroundModelEnabled(true);
    recognizer.SetOrder(128);
    auto trainData = std::make_shared<SpeechData>();
    trainData->Load("trainubm.txt");
    trainData->Normalize();
    auto testData = std::make_shared<SpeechData>();
    std::string claimedSpeaker = "225";
    recognizer.Train(trainData);   
    std::ofstream recresults;
    recresults.open("recresults.txt", std::ios::app);  
    std::vector<Real> verification_results;
    std::string testFile;
    

    testFile = "test.txt";
    claimedSpeaker = "225";
    testData->Load(testFile);
    testData->Normalize();
    verification_results = recognizer.Verify(claimedSpeaker, testData);
    for (auto& entry : verification_results)
    {
        recresults << entry << " ";
    }

    recresults << std::endl;

    testFile = "test2.txt";
    testData->Load(testFile);
    testData->Normalize();
    verification_results = recognizer.Verify(claimedSpeaker, testData);
    for (auto& entry : verification_results)
    {
        recresults << entry << " ";
    } 

    recresults.close();
}
