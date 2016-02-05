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
    {
        std::cout << "VQ Test" << std::endl;

        VQRecognizer recognizer;

        //GMMRecognizer recognizer;
        //ANNRecognizer recognizer;

        recognizer.SetBackgroundModelEnabled(true);
        recognizer.SetOrder(128);

        std::map<std::string, RecognitionResult> result;

        auto trainData = std::make_shared<SpeechData>();
        trainData->Load("train.txt");
        trainData->Normalize();

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

        std::cout << "done" << std::endl;
    }

    //system("pause");
}
