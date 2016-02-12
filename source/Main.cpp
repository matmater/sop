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
        std::string file = "samples/samples_" + toString(sf+i) + ".txt";
        std::cout << file << std::endl;
        data->Load(file, sl, gl, train);
    }
    data->Validate();
    data->Normalize();
}

void Evaluate(std::map<std::string, RecognitionResult>& results, std::vector<unsigned int>& finalresults)
{
    unsigned int correctlyRecognized = 0;
    unsigned int correctlyRejected = 0;
    unsigned int incorrectlyRecognized = 0;
    unsigned int incorrectlyRejected = 0;

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
      
    finalresults[0] += correctlyRecognized;
    finalresults[1] += correctlyRejected;   
    finalresults[2] += incorrectlyRecognized;    
    finalresults[3] += incorrectlyRejected;
}

void RecognitionTest()
{
    unsigned int sf = 240;  // startfile, speaker to start with
    unsigned int gf = 2;    // getfiles, number of files to get
    unsigned int sl = 1;    // startline, line to start at
    unsigned int gl = 4;    // getlines, number of lines to get
    unsigned int trainingCycles = 2;
    unsigned int correctClaimed = 5;
    
    std::cout << "Recognition Test" << std::endl;
    VQRecognizer recognizer;
    recognizer.SetBackgroundModelEnabled(false);
    recognizer.SetOrder(128);
    recognizer.SetWeightingEnabled(false);
    //recognizer.SetScoreNormalizationType(NormalizationType::ZERO_TEST);    
    auto trainData = std::make_shared<SpeechData>();
    auto testData = std::make_shared<SpeechData>();
    std::map<std::string, RecognitionResult> result;
    std::vector<unsigned int> finalRecResults (4,0);
    
    for (unsigned int i = 0; i < trainingCycles ; i++)
    {
        std::cout << i+1 << "/" << trainingCycles << std::endl;
        LoadTextSamples(trainData, sf, gf, sl, gl, true);
        //recognizer.SetImpostorSpeakerData(trainData);
        recognizer.Train(trainData);
 
        //auto start = std::chrono::system_clock::now();            
        LoadTextSamples(testData, sf, gf, sl+gl, correctClaimed, false);                      
        //auto end = std::chrono::system_clock::now();
        //std::chrono::duration<double> loadDuration = end - start;            
        //start = std::chrono::system_clock::now();
        recognizer.Test(testData, result);
        //end = std::chrono::system_clock::now();
        //std::chrono::duration<double> testDuration = end - start;
        Evaluate(result, finalRecResults);
        sf+=gl;
    }
        
    unsigned int sum = 0;
    for (unsigned int n : finalRecResults)
        sum += n;
    
std::cout << "Correctly recognized:   " << finalRecResults[0] << " " << 100.0 * finalRecResults[0] / sum << "%" << std::endl
              << "Correctly rejected:     " << finalRecResults[1] << " " << 100.0 * finalRecResults[1] / sum << "%" << std::endl
              << "Incorrectly recognized: " << finalRecResults[2] << " " << 100.0 * finalRecResults[2] / sum << "%" << std::endl          
              << "Incorrectly rejected:   " << finalRecResults[3] << " " << 100.0 * finalRecResults[3] / sum << "%" << std::endl;
              
    std::ofstream resultss;
    resultss.open("recognitionresults.txt", std::ios::app);
    resultss << finalRecResults[0] << " " << finalRecResults[1] << " " << finalRecResults[2] << " " << finalRecResults[3] << std::endl;
    //loadDur.count()
    resultss.close();    
}

void VerificationTest()
{
    unsigned int sf = 240;  // startfile, speaker to start with
    unsigned int gf = 2;    // getfiles, number of files to get
    unsigned int sl = 1;    // startline, line to start at
    unsigned int gl = 5;    // getlines, number of lines to get
    unsigned int trainingCycles = 5;
    unsigned int incorrectClaimed = 5;
    unsigned int correctClaimed = 5;
   
    std::cout << "Verification test" << std::endl;
    VQRecognizer recognizer;
    recognizer.SetBackgroundModelEnabled(false);
    recognizer.SetOrder(128);
    recognizer.SetWeightingEnabled(false);
    recognizer.SetScoreNormalizationType(NormalizationType::ZERO_TEST);
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
        LoadTextSamples(trainData, sf, gf, sl, gl, true);
        recognizer.Train(trainData);
        LoadTextSamples(testData2, sf+gf, incorrectClaimed, sl, gl, false);          
        for (unsigned int j = 0; j < gf; j++)
        {
            LoadTextSamples(testData, sf+j, 1, sl+gl, correctClaimed, false);          
            claimedSpeaker = toString(sf+j);
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
    vresults.close();    
}

int main(int argc, char** argv)
{
    RecognitionTest();
    //VerificationTest();
    
    /*
    {
        std::cout << "Verification Test" << std::endl;
        GMMRecognizer recognizer;
        // GMMRecognizer recognizer;
        // ANNRecognizer recognizer;

        auto trainData = std::make_shared<SpeechData>();
        trainData->Load("train.txt");
        trainData->Normalize();
        recognizer.SetBackgroundModelEnabled(true);
        recognizer.SetOrder(128);
        // recognizer.SetWeightingEnabled(true);
        recognizer.SetScoreNormalizationType(NormalizationType::ZERO_TEST);
        recognizer.SetImpostorSpeakerData(trainData);
        std::map<std::string, RecognitionResult> result;
        std::vector<unsigned int> finalRecResults (4,0);
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
        Evaluate(result, finalRecResults);
    }
    */   
}
