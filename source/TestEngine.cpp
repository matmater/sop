#include "TestEngine.h"

#include "ModelRecognizer.h"
#include "SpeechData.h"
#include "VQRecognizer.h"
#include "GMMRecognizer.h"

TestEngine::TestEngine()
{

}

void TestEngine::Recognize(
    const std::string& id,
    std::shared_ptr<ModelRecognizer> recognizer,
    unsigned int sf,
    unsigned int gf,
    unsigned int sl,
    unsigned int gl,
    unsigned int cycles)
{
    // \todo CHECK BUGS!

    std::ofstream results(id + "_rec_" + GetIdentifier(recognizer) + ".txt");

    auto testData = std::make_shared<SpeechData>();

    // Test utterances
    LoadTextSamples(id, testData, sf, cycles * gf, sl, gl, false);

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

    std::ofstream results(id + "_ver_" + GetIdentifier(recognizer) + ".txt");

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

        LoadTextSamples(id, testData2, sf+gf, incorrectClaimed, sl, gl, false);          
        for (unsigned int j = 0; j < gf; j++)
        {
            std::cout << "Verification check " << j+1 << "/" << gf << std::endl;
        
            LoadTextSamples(id, testData, sf+j, 1, sl+gl, correctClaimed, false);          
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