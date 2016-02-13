#include <fstream>
#include "Common.h"
#include <chrono>

#include "GMMRecognizer.h"
#include "VQRecognizer.h"

#define INDEXFIX
#undef INDEXFIX

#ifdef INDEXFIX
static std::vector<std::string> speakerFiles;
#endif

bool FileExists(const std::string& path)
{
    std::ifstream file(path);

    return file.good();
}

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
#ifdef INDEXFIX
        std::cout << speakerFiles[i+sf-225] << std::endl;
        data->Load(speakerFiles[i+sf-225], sl, gl, train, toString(sf+i));
#else
        std::string file = "samples/samples_" + toString(sf+i) + ".txt";
        std::cout << file << std::endl;
        data->Load(file, sl, gl, train);
#endif
    }
    data->Validate();
    data->Normalize();
}

void RecognitionTest()
{
    unsigned int sf = 240;  // startfile, speaker to start with
    unsigned int gf = 5;    // getfiles, number of files to get
    unsigned int sl = 1;    // startline, line to start at
    unsigned int gl = 3;    // getlines, number of lines to get
    unsigned int trainingCycles = 4;
    unsigned int correctClaimed = 5;
    
    std::ofstream results;
    results.open("recognitionresults.txt", std::ios::app);

    std::cout << "Recognition Test" << std::endl;

    //recognizer.SetScoreNormalizationType(ScoreNormalizationType::ZERO_TEST);
    auto trainData = std::make_shared<SpeechData>();
    auto testData = std::make_shared<SpeechData>();
    
    // Test utterances
    LoadTextSamples(testData, sf, trainingCycles * 10, gl+1, gl, false); // Not sure about that +1

    // Speaker model data
    LoadTextSamples(trainData, sf, trainingCycles * 10, 1, gl, true);
    
    VQRecognizer recognizer;
    recognizer.SetBackgroundModelEnabled(false);
    recognizer.SetOrder(128);
    recognizer.SetWeightingEnabled(false);
    recognizer.SetSpeakerData(trainData);

    recognizer.Train();

    // Only after training models can be selected for testing.

    std::vector<unsigned int> finalRecResults (2,0);
    
    for (unsigned int i = 1; i <= trainingCycles ; i++)
    {
        std::cout << i << "/" << trainingCycles << std::endl;
        
        unsigned int population = i * 10;
        unsigned int correct = 0;
        unsigned int incorrect = 0;
        
        // Select trained speakers.
        std::vector<SpeakerKey> speakers;
        for (unsigned int k = 0; k < population; ++k)
        {
            SpeakerKey key(toString(sf + k));

            // Debug check.
            if (trainData->GetSamples().find(key) == trainData->GetSamples().end())
            {
                std::cout << "Speaker '" << key << "'not loaded." << std::endl;
                continue;
            }

            speakers.push_back(key);
        }

        recognizer.SelectSpeakerModels(speakers);
        
        // Check test data against selected speakers.
        // Slow but works.
        for (const auto& samples : testData->GetSamples())
        {
            for (const auto& speaker : speakers)
            {
                if (samples.first.IsSameSpeaker(speaker))
                {
                    if (recognizer.IsRecognized(speaker, samples.second))
                    {
                        ++correct;
                    }

                    else
                    {
                        ++incorrect;
                    }

                    break;
                }
            }
        }

        if (correct > 0 || incorrect > 0)
        {
            std::cout << "Speakers " << population
                      << ", accuracy "
                      << 100.0f * static_cast<float>(correct) / static_cast<float>(correct + incorrect) << "%" << std::endl;
        }

        results << population << " " << correct << " " << incorrect << std::endl;
    }
    
    results.close();
}

void VerificationTest()
{
    unsigned int sf = 240;  // startfile, speaker to start with
    unsigned int gf = 5;    // getfiles, number of files to get
    unsigned int sl = 1;    // startline, line to start at
    unsigned int gl = 3;    // getlines, number of lines to get
    unsigned int trainingCycles = 3;
    unsigned int incorrectClaimed = 5;
    unsigned int correctClaimed = 5;

    unsigned int ubmSpeakerCount = 10; // Max ~ 30.
    unsigned int ubmLineCount = 2;

    unsigned int impostorSpeakerCount = 5; // Max = 20.
    // Line count same as gl.
   
    std::cout << "Verification test" << std::endl;

    auto trainData = std::make_shared<SpeechData>();
    auto testData = std::make_shared<SpeechData>();
    auto testData2 = std::make_shared<SpeechData>();
    auto ubmData = std::make_shared<SpeechData>();

    std::string claimedSpeaker;
    std::ofstream vresults;
    vresults.open("verificationresults.txt", std::ios::app);
    std::vector<Real> verificationResults;
    
    // Load background model data.
    LoadTextSamples(ubmData, sf + 70, ubmSpeakerCount, sl, gl, true);

    // Load ALL speakers to be tested (including impostors).
    LoadTextSamples(trainData, sf, gf * trainingCycles + impostorSpeakerCount, sl, gl, true);
    
    VQRecognizer recognizer;
    recognizer.SetBackgroundModelEnabled(true);
    recognizer.SetOrder(256);
    recognizer.SetAdaptationIterations(2);
    //recognizer.SetWeightingEnabled(true);
    recognizer.SetScoreNormalizationType(ScoreNormalizationType::ZERO_TEST);

    recognizer.SetBackgroundModelData(ubmData);
    recognizer.SetSpeakerData(trainData);

    recognizer.Train();

    // Only after training models can be selected for testing.
    
    //Avoid training the background model again:
    //recognizer.SetBackgroundModelTrainingEnabled(true);
    //recognizer.Train();
    //recognizer.SetBackgroundModelTrainingEnabled(false);
    //...
    //recognizer.SetSpeakerData(...)
    //recognizer.Train()
    //recognizer.SelectSpeakerModels(...)
    //recognizer.SelectImpostorModels(...)
    //...

    // Select trained impostors.
    std::vector<SpeakerKey> impostors;
    for (unsigned int i = 0; i < impostorSpeakerCount; ++i)
    {
        SpeakerKey key(toString(sf + gf * trainingCycles + i));

        // Debug check.
        if (trainData->GetSamples().find(key) == trainData->GetSamples().end())
        {
            std::cout << "Impostor '" << key << "'not loaded." << std::endl;
            continue;
        }

        impostors.push_back(key);
    }
    recognizer.SelectImpostorModels(impostors);
    
    for (unsigned int i = 0; i < trainingCycles ; i++)
    {
        std::cout << i+1 << "/" << trainingCycles << std::endl;
        
        // Select trained speakers.
        std::vector<SpeakerKey> speakers;
        for (unsigned int k = 0; k < gf; ++k)
        {
            SpeakerKey key(toString(sf + k));

            // Debug check.
            if (trainData->GetSamples().find(key) == trainData->GetSamples().end())
            {
                std::cout << "Speaker '" << i << "'not loaded." << std::endl;
                continue;
            }

            speakers.push_back(key);
        }
        recognizer.SelectSpeakerModels(speakers);

        // Test

        LoadTextSamples(testData2, sf+gf, incorrectClaimed, sl, gl, false);          
        for (unsigned int j = 0; j < gf; j++)
        {
            std::cout << "Verification check " << j+1 << "/" << gf << std::endl;
        
            LoadTextSamples(testData, sf+j, 1, sl+gl, correctClaimed, false);          
            claimedSpeaker = toString(sf+j);
            verificationResults = recognizer.Verify(SpeakerKey(claimedSpeaker), testData);
            for (auto& entry : verificationResults)
            {
                vresults << entry << " ";
            }
            vresults << std::endl;
            verificationResults = recognizer.Verify(SpeakerKey(claimedSpeaker), testData2);
            for (auto& entry : verificationResults)
            {
                vresults << entry << " ";
            }
            vresults << std::endl;
        }
        sf += gf;
    }
}

int main(int argc, char** argv)
{
#ifdef INDEXFIX
    for (unsigned int i = 225; i <= 376; ++i)
    {
        std::string fname = "samples/samples_" + toString(i) + ".txt";

        if (FileExists(fname))
        {
            speakerFiles.push_back(fname);
        }
    }
#endif

    RecognitionTest();
    //VerificationTest();
}