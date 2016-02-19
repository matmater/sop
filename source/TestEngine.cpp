#include "TestEngine.h"

#include "ModelRecognizer.h"
#include "SpeechData.h"
#include "VQRecognizer.h"
#include "GMMRecognizer.h"

TestEngine::TestEngine()
{

}

void TestEngine::Run(const std::string& file)
{
    std::vector<Test> tests;

    std::ifstream fs(file);

    std::string line;

    std::string currentTestId;

    TestType testType = TestType::UNKNOWN;
    
    unsigned int ubmSf = 0;
    unsigned int ubmGf = 0;
    unsigned int ubmSl = 0;
    unsigned int ubmGl = 0;

    while(std::getline(fs, line))
    {
        std::string item;
        
        std::stringstream ssLine(line);

        if (!(ssLine >> item))
        {
            continue;
        }

        if (item == "%ubm")
        {
            if (!(ssLine >> ubmSf)) {
                std::cerr << "Missing UBM sf." << std::endl;
                return;
            }
            if (!(ssLine >> ubmGf)) {
                std::cout << "Missing UBM gf." << std::endl;
                return;
            }
            if (!(ssLine >> ubmSl)) {
                std::cout << "Missing UBM sl." << std::endl;
                return;
            }
            if (!(ssLine >> ubmGl)) {
                std::cout << "Missing UBM gl." << std::endl;
                return;
            }

            continue;
        }

        if (item.size() > 1 && item[0] == '%')
        {
            std::string ttype;

            if (ssLine >> ttype)
            {
                if (ttype == "rec")
                {
                    testType = TestType::RECOGNITION;
                }

                else if (ttype == "ver")
                {
                    testType = TestType::VERIFICATION;
                }

                else
                {
                    std::cout << "Missing test type." << std::endl;
                    return;
                }
            }

            currentTestId = item.erase(0, 1);
            std::cout << "Current test id: '" << currentTestId << "', type: '" << ttype << "'." << std::endl;
            continue;
        }

        if (testType != TestType::UNKNOWN)
        {
            std::string features = item;
        
            std::string recognizerType;
        
            if (!(ssLine >> recognizerType))
            {
                std::cout << "Invalid test file: recognizer type not specified." << std::endl;
                return;
            }

            Test test;
            
            // Training.
            if (!(ssLine >> test.trainSf)) {
                std::cout << "Missing 'trainSf'." << std::endl;
                return;
            }

            if (!(ssLine >> test.trainGf)) {
                std::cout << "Missing 'trainGf'." << std::endl;
                return;
            }

            if (!(ssLine >> test.trainSl)) {
                std::cout << "Missing 'trainSl'." << std::endl; 
                return;
            }
        
            if (!(ssLine >> test.trainGl)) {
                std::cout << "Missing 'trainGl'." << std::endl;
                return;
            }
            
            // Testing.
            if (!(ssLine >> test.testSf)) {
                std::cout << "Missing 'testSf'." << std::endl;
                return;
            }

            if (!(ssLine >> test.testGf)) {
                std::cout << "Missing 'testGf'." << std::endl;
                return;
            }

            if (!(ssLine >> test.testSl)) {
                std::cout << "Missing 'testSl'." << std::endl;
                return;
            }
        
            if (!(ssLine >> test.testGl)) {
                std::cout << "Missing 'testGl'." << std::endl;
                return;
            }

            // Cycles.
            if (!(ssLine >> test.cycles)) {
                std::cout << "Missing 'cycles'." << std::endl;
                return;
            }

            if (testType == TestType::VERIFICATION)
            {
                if (!(ssLine >> test.incorrectClaimed)) {
                    std::cout << "Missing 'incorrectClaimed'." << std::endl;
                    return;
                }

                if (!(ssLine >> test.correctClaimed)) {
                    std::cout << "Missing 'correctClaimed'." << std::endl;
                    return;
                }
        
                if (!(ssLine >> test.si)) {
                    std::cout << "Missing 'si'." << std::endl;
                    return;
                }

                if (!(ssLine >> test.gi)) {
                    std::cout << "Missing 'gi'." << std::endl;
                    return;
                }
            }

            std::string feature;

            while (ssLine >> feature)
            {
                if (feature == "-z") {
                    test.scoreNormalizationType = ScoreNormalizationType::ZERO;
                } else if (feature == "-t") {
                    test.scoreNormalizationType = ScoreNormalizationType::TEST;
                } else if (feature == "-zt") {
                    test.scoreNormalizationType = ScoreNormalizationType::ZERO_TEST;
                } else if (feature == "-tz") {
                    test.scoreNormalizationType = ScoreNormalizationType::TEST_ZERO;
                } else if (feature == "-w") {
                    test.weighting = true;
                } else if (feature == "-ubm") {
                    test.ubm = true;
                } else if (feature == "-o") {
                    if (!(ssLine >> test.order)) {
                        std::cout << "Error: invalid order." << std::endl;
                        return;
                    }
                }
            }

            if (recognizerType == "vq"){
                test.recognizerType = RecognizerType::VQ;
            } else if (recognizerType == "gmm") {
                test.recognizerType = RecognizerType::GMM;
            } else {
                std::cout << "Unknown recognizer type '" << recognizerType << "'." << std::endl;
                return;
            }

            test.id = currentTestId;
            test.features = features;
            test.type = testType;

            tests.push_back(test);
        }
    }

    std::sort(tests.begin(), tests.end(), [](const Test& a, const Test& b) {
        if (a.features < b.features) return true;
        if (a.features > b.features) return false;
        
        // Train data
        if (a.trainSf < b.trainSf) return true;
        if (a.trainSf > b.trainSf) return false;

        if (a.trainGf < b.trainGf) return true;
        if (a.trainGf > b.trainGf) return false;

        if (a.trainSl < b.trainSl) return true;
        if (a.trainSl > b.trainSl) return false;
        
        if (a.trainGl < b.trainGl) return true;
        if (a.trainGl > b.trainGl) return false;

        // Order
        if (a.order < b.order) return true;
        if (a.order > b.order) return false;
        
        if (a.scoreNormalizationType < b.scoreNormalizationType) return true;
        if (a.scoreNormalizationType > b.scoreNormalizationType) return false;
        
        if (a.weighting < b.weighting) return true;
        if (a.weighting > b.weighting) return false;

        if (a.ubm < b.ubm) return true;
        if (a.ubm > b.ubm) return false;
        
        return (a.recognizerType < b.recognizerType);
    });

    std::string previousFeatures;

    std::shared_ptr<SpeechData> ubmData;
    std::shared_ptr<SpeechData> trainData;
    
    unsigned int previousUbmSf = 0;
    unsigned int previousUbmGf = 0;
    unsigned int previousUbmSl = 0;
    unsigned int previousUbmGl = 0;

    std::shared_ptr<ModelRecognizer> recognizer;
    
    std::shared_ptr<VQRecognizer> vq = std::make_shared<VQRecognizer>();
    std::shared_ptr<GMMRecognizer> gmm = std::make_shared<GMMRecognizer>();

    auto previousIt = tests.end();
    for (auto it = tests.begin(); it != tests.end(); it++)
    {
        if (it->features != previousIt->features ||
            it->trainSf != previousIt->trainSf ||
            it->trainGf != previousIt->trainGf ||
            it->trainSl != previousIt->trainSl ||
            it->trainGl != previousIt->trainGl)
        {
            // Load speaker data.
            trainData = std::make_shared<SpeechData>();
            LoadTextSamples(
                it->features,
                trainData,
                it->trainSf,
                it->trainGf,
                it->trainSl,
                it->trainGl,
                true
            );
        }

        if (it->features != previousIt->features)
        {
            // Load ubm data from a different set of speakers.
            ubmData = std::make_shared<SpeechData>();
            LoadTextSamples(it->features, ubmData, ubmSf, ubmGf, ubmSl, ubmGl, true);
        }

        if (it->recognizerType == RecognizerType::VQ)
        {
            vq->SetWeightingEnabled(it->weighting);
            recognizer = vq;
        }

        else if (it->recognizerType == RecognizerType::GMM)
        {
            recognizer = gmm;
        }

        else
        {
            std::cout << "Unknown recognizer type." << std::endl;
            return;
        }

        vq->SetOrder(it->order);
        vq->SetBackgroundModelEnabled(it->ubm);
        vq->SetScoreNormalizationType(it->scoreNormalizationType);
        
        recognizer->SetSpeakerData(trainData);
        recognizer->SetBackgroundModelData(ubmData);

        if (it->type == TestType::RECOGNITION)
        {
            Recognize(
                it->id, it->features, recognizer,
                it->testSf,            // sf
                it->testGf,            // gf
                it->testSl,            // sl
                it->testGl,            // gl
                it->cycles             // cycles
            );
        }

        else if (it->type == TestType::RECOGNITION)
        {
            Verify(
                it->id, it->features, recognizer,
                it->testSf,            // sf
                it->testGf,            // gf
                it->testSl,            // sl
                it->testGl,            // gl
                it->cycles,            // cycles
                it->incorrectClaimed,  // incorrectClaimed
                it->correctClaimed,    // correctClaimed
                it->si,                // si (start-impostor)
                it->gi                 // gi (get-impostor)
            );
        }

        else
        {
            std::cout << "Unknown test type." << std::endl;
        }

        previousIt = it;
    }
}

void TestEngine::Recognize(
    const std::string& id,
    const std::string& features,
    std::shared_ptr<ModelRecognizer> recognizer,
    unsigned int sf,
    unsigned int gf,
    unsigned int sl,
    unsigned int gl,
    unsigned int cycles)
{
    // \todo CHECK BUGS!

    std::ofstream results(id + "_" + features + "_rec_" + GetIdentifier(recognizer) + ".txt");

    auto testData = std::make_shared<SpeechData>();

    // Test utterances
    LoadTextSamples(features, testData, sf, cycles * gf, sl, gl, false);

    for (unsigned int i = 1; i <= cycles ; i++)
    {
        std::cout << i << "/" << cycles << std::endl;
        
        unsigned int population = i * 10;
        unsigned int correct = 0;
        unsigned int incorrect = 0;
        
        // Select trained speakers.
        std::vector<SpeakerKey> speakers;
        for (unsigned int k = 0; k < population; ++k)
        {
            SpeakerKey key(toString(sf + k));

            // Debug check.
            if (recognizer->GetSpeakerData()->GetSamples().find(key)
               == recognizer->GetSpeakerData()->GetSamples().end())
            {
                std::cout << "Speaker '" << key << "' not loaded." << std::endl;
                continue;
            }

            speakers.push_back(key);
        }

        recognizer->SelectSpeakerModels(speakers);
        
        // Check test data against selected speakers.
        // Slow but works.
        for (const auto& samples : testData->GetSamples())
        {
            for (const auto& speaker : speakers)
            {
                if (samples.first.IsSameSpeaker(speaker))
                {
                    if (recognizer->IsRecognized(speaker, samples.second))
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
}

void TestEngine::Verify(
    const std::string& id,
    const std::string& features,
    std::shared_ptr<ModelRecognizer> recognizer,
    unsigned int sf,
    unsigned int gf,
    unsigned int sl,
    unsigned int gl,
    unsigned int cycles,
    unsigned int incorrectClaimed,
    unsigned int correctClaimed,
    unsigned int si,
    unsigned int gi)
{
    // \todo CHECK BUGS!

    std::ofstream results(id + "_" + features + "_ver_" + GetIdentifier(recognizer) + ".txt");

    // Select trained impostors.
    std::vector<SpeakerKey> impostors;
    for (unsigned int i = 0; i < gi; ++i)
    {
        SpeakerKey key(toString(si + i));

        // Debug check.
        if (recognizer->GetSpeakerData()->GetSamples().find(key)
            == recognizer->GetSpeakerData()->GetSamples().end())
        {
            std::cout << "Impostor '" << key << "' not loaded." << std::endl;
            continue;
        }

        impostors.push_back(key);
    }
    recognizer->SelectImpostorModels(impostors);
    
    auto testData = std::make_shared<SpeechData>();
    auto testData2 = std::make_shared<SpeechData>();
    
    for (unsigned int i = 0; i < cycles ; i++)
    {
        std::cout << i+1 << "/" << cycles << std::endl;
        
        // Select trained speakers.
        std::vector<SpeakerKey> speakers;
        for (unsigned int k = 0; k < gf; ++k)
        {
            SpeakerKey key(toString(sf + k));

            // Debug check.
            if (recognizer->GetSpeakerData()->GetSamples().find(key)
               == recognizer->GetSpeakerData()->GetSamples().end())
            {
                std::cout << "Speaker '" << sf + k << "' not loaded." << std::endl;
                continue;
            }

            speakers.push_back(key);
        }
        recognizer->SelectSpeakerModels(speakers);

        // Test

        LoadTextSamples(features, testData2, sf+gf, incorrectClaimed, sl, gl, false);          
        for (unsigned int j = 0; j < gf; j++)
        {
            std::cout << "Verification check " << j+1 << "/" << gf << std::endl;
        
            LoadTextSamples(features, testData, sf+j, 1, sl+gl, correctClaimed, false);          
            std::string claimedSpeaker = toString(sf+j);
            auto verificationResults = recognizer->Verify(SpeakerKey(claimedSpeaker), testData);
            for (auto& entry : verificationResults)
            {
                results << entry << " ";
            }
            results << std::endl;
            verificationResults = recognizer->Verify(SpeakerKey(claimedSpeaker), testData2);
            for (auto& entry : verificationResults)
            {
                results << entry << " ";
            }
            results << std::endl;
        }
        sf += gf;
    }
}

std::string TestEngine::GetIdentifier(std::shared_ptr<ModelRecognizer> recognizer)
{
    // \todo Add more identifiers.

    std::stringstream ss;

    if (dynamic_cast<VQRecognizer*>(recognizer.get()) != nullptr)
    {
        ss << "vq";
    }

    else if (dynamic_cast<GMMRecognizer*>(recognizer.get()) != nullptr)
    {
        ss << "gmm";
    }

    else
    {
        ss << "unknown";
    }
    
    ss << "_o" << recognizer->GetOrder();

    if (dynamic_cast<VQRecognizer*>(recognizer.get()) != nullptr)
    {
        if (static_cast<VQRecognizer*>(recognizer.get())->IsWeightingEnabled())
        {
            ss << "_weighted";
        }
    }

    if (recognizer->IsBackgroundModelEnabled())
    {
        ss << "_ubm";
    }

    return ss.str();
}