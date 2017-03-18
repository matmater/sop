/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#include "TestEngine.h"

#include "ModelRecognizer.h"
#include "SpeechData.h"
#include "VQRecognizer.h"
#include "GMMRecognizer.h"
#include "Timer.h"

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

    std::map<std::string, TestHeader> testIds;

    unsigned int indexCounter = 0;

    while(std::getline(fs, line))
    {
        std::string item;

        std::stringstream ssLine(line);

        if (!(ssLine >> item))
        {
            continue;
        }

        // Skip comments.
        if (item.size() >= 2 && item[0] == '/' && item[1] == '/')
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

                else if (ttype == "recpop")
                {
                    testType = TestType::RECOGNITION_POPULATION;
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

            std::string testLabel;

            GetStringLiteral(ssLine, testLabel);

            currentTestId = item.erase(0, 1);

            std::cout << "Current test id: '" << currentTestId << "', type: '" << ttype << "', label: '" << testLabel << "'." << std::endl;

            if (testIds.find(currentTestId) != testIds.end())
            {
                std::cout << "Duplicate test identifiers." << std::endl;
                return;
            }

            std::string target = "";

            ssLine >> target;

            TestHeader testHeader;
            testHeader.type = testType;
            testHeader.label = testLabel;
            testHeader.targetId = target;
            testIds[currentTestId] = testHeader;

            indexCounter = 0;

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
                // Stop at comment.
                if (item.size() >= 2 && item[0] == '/' && item[1] == '/')
                {
                    break;
                }

                if (feature == "-z") {
                    test.scoreNormalizationType = ScoreNormalizationType::ZERO;
                } else if (feature == "-t") {
                    test.scoreNormalizationType = ScoreNormalizationType::TEST;
                } else if (feature == "-zt") {
                    test.scoreNormalizationType = ScoreNormalizationType::ZERO_TEST;
                } else if (feature == "-tz") {
                    test.scoreNormalizationType = ScoreNormalizationType::TEST_ZERO;
                } else if (feature == "-wt") {
                    test.weighting = true;
                } else if (feature == "-ubm") {
                    test.ubm = true;
                } else if (feature == "-o") {
                    if (!(ssLine >> test.order)) {
                        std::cout << "Error: invalid order." << std::endl;
                        return;
                    }
                } else if (feature == "-mul") {
                    if (!(ssLine >> test.multiplier) || test.multiplier == 0) {
                        std::cout << "Error: invalid multiplier." << std::endl;
                        return;
                    }
                } else if (feature == "-label") {
                    if (!(GetStringLiteral(ssLine, test.label))) {
                        std::cout << "Error: invalid test label." << std::endl;
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
            test.index = indexCounter;

            tests.push_back(test);

            ++indexCounter;
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

    for (const auto& test : testIds)
    {
        std::string label;

        if (!test.second.label.empty())
        {
            label = test.second.label;
        }

        else
        {
            label = test.first;
        }

        if (test.second.type == TestType::RECOGNITION || test.second.type == TestType::RECOGNITION_POPULATION)
        {
            // Initialize recognition results file.
            std::ofstream resultsFile(test.first + "_rec.txt");

            if (test.second.targetId.empty())
            {
                // Just init (and clear) the test file.
                std::ofstream testFile(test.first + ".test");
                testFile << label << "|" << "rec" << std::endl;
            }

            std::ofstream perfTrainFile(test.first + ".perftrain");
            perfTrainFile << label << "|" << "rec" << std::endl;

            std::ofstream perfTestFile(test.first + ".perftest");
            perfTestFile << label << "|" << "rec" << std::endl;

        } else if (test.second.type == TestType::VERIFICATION) {
            if (!test.second.targetId.empty())
            {
                std::cout << "Targets are only supported in recognition." << std::endl;
                return;
            }
            else
            {
                // Just init (and clear) the test file.
                std::ofstream testFile(test.first + ".test");
                testFile << label << "|" << "ver" << std::endl;

                std::ofstream perfTrainFile(test.first + ".perftrain");
                perfTrainFile << label << "|" << "ver" << std::endl;

                std::ofstream perfTestFile(test.first + ".perftest");
                perfTestFile << label << "|" << "ver" << std::endl;
            }
        }
        else {
            std::cout << "Invalid test id." << std::endl;
            return;
        }
    }

    for (const auto& test : testIds)
    {
        if (test.second.type == TestType::RECOGNITION || test.second.type == TestType::RECOGNITION_POPULATION)
        {
            if (!test.second.targetId.empty())
            {
                std::ofstream testFile(test.second.targetId + ".test", std::ios_base::app);
                testFile << test.first + "_rec.txt" << "|" << test.second.label << std::endl;
            }
        }
    }

    for (auto it = tests.begin(); it != tests.end(); it++)
    {
        if (previousIt   == tests.end()          ||
            it->features != previousIt->features ||
            it->trainSf  != previousIt->trainSf  ||
            it->trainGf  != previousIt->trainGf  ||
            it->trainSl  != previousIt->trainSl  ||
            it->trainGl  != previousIt->trainGl)
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
                it->multiplier,
                true
            );
        }

        if (previousIt   == tests.end() ||
            it->features != previousIt->features)
        {
            // Load ubm data from a different set of speakers.
            ubmData = std::make_shared<SpeechData>();
            LoadTextSamples(it->features, ubmData, ubmSf, ubmGf, ubmSl, ubmGl, 1, true);
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

        recognizer->SetOrder(it->order);
        recognizer->SetBackgroundModelEnabled(it->ubm);
        recognizer->SetScoreNormalizationType(it->scoreNormalizationType);

        recognizer->SetSpeakerData(trainData);
        recognizer->SetBackgroundModelData(ubmData);

        if (it->type == TestType::RECOGNITION)
        {
            std::cout << "Recognition test: " << GetLabel(*it) << std::endl;

            recognizer->SetBackgroundModelEnabled(it->ubm);
            recognizer->SetAdaptationEnabled(it->ubm);

            std::ofstream perfTrainFile(it->id + ".perftrain", std::ios_base::app);

            perfTrainFile << it->id + "_rec" + ".txt" << "|"
                << GetLabel(*it) << "|"
                << recognizer->GetTrainTimeSpeakerModels() << "|"
                << recognizer->GetSpeakerData()->GetSamples().size() << "|"
                << recognizer->GetTrainTimeBackgroundModel() << "|"
                << recognizer->GetBackgroundModelData()->GetSamples().size() << std::endl;

            Recognize(*it, recognizer);
        }

        else if (it->type == TestType::RECOGNITION_POPULATION)
        {
            std::cout << "Recognition (population) test: " << GetLabel(*it) << std::endl;

            recognizer->SetBackgroundModelEnabled(it->ubm);
            recognizer->SetAdaptationEnabled(it->ubm);

            std::ofstream perfTrainFile(it->id + ".perftrain", std::ios_base::app);

            perfTrainFile << it->id + "_rec" + ".txt" << "|"
                << GetLabel(*it) << "|"
                << recognizer->GetTrainTimeSpeakerModels() << "|"
                << recognizer->GetSpeakerData()->GetSamples().size() << "|"
                << recognizer->GetTrainTimeBackgroundModel() << "|"
                << recognizer->GetBackgroundModelData()->GetSamples().size() << std::endl;

            RecognizePop(*it, recognizer);
        }

        else if (it->type == TestType::VERIFICATION)
        {
            std::cout << "Verification test: " << GetLabel(*it) << std::endl;

            recognizer->SetBackgroundModelEnabled(it->ubm);
            recognizer->SetAdaptationEnabled(it->ubm);

            std::ofstream perfTrainFile(it->id + ".perftrain", std::ios_base::app);

            perfTrainFile << it->id + "_" + toString(it->index) + "_ver.txt" << "|"
                << GetLabel(*it) << "|"
                << recognizer->GetTrainTimeSpeakerModels() << "|"
                << recognizer->GetSpeakerData()->GetSamples().size() << "|"
                << recognizer->GetTrainTimeBackgroundModel() << "|"
                << recognizer->GetBackgroundModelData()->GetSamples().size() << std::endl;

            Verify(*it, recognizer);
        }

        else
        {
            std::cout << "Unknown test type." << std::endl;
        }

        previousIt = it;
    }
}

void TestEngine::RecognizePop(
    const Test& test,
    std::shared_ptr<ModelRecognizer> recognizer)
{
    std::string resultsFileName = test.id + "_rec" + ".txt";

    std::ofstream results(resultsFileName, std::ios_base::app);

    auto testData = std::make_shared<SpeechData>();

    // Test utterances
    LoadTextSamples(test.features, testData, test.testSf, test.cycles * test.testGf, test.testSl, test.testGl, test.multiplier, false);

    for (unsigned int i = 1; i <= test.cycles ; i++)
    {
        Real realTestTime = 0.0f;

        std::cout << i << "/" << test.cycles << std::endl;

        unsigned int correct = 0;
        unsigned int incorrect = 0;

        // Select trained speakers.
        std::vector<SpeakerKey> speakers;
        for (unsigned int k = 0; k < test.testGf; ++k)
        {
            SpeakerKey key(GetSpeakerString(test.testSf + k, test.features));

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

        Timer timer;
        for (const auto& samples : testData->GetSamples())
        {
            std::string speakerString;
            speakerString += samples.first.GetId()[0];
            speakerString += samples.first.GetId()[1];
            speakerString += samples.first.GetId()[2];

            if (recognizer->IsRecognized(SpeakerKey(speakerString), samples.second))
            {
                ++correct;
            }

            else
            {
                ++incorrect;
            }
        }
        realTestTime += timer.GetTimeElapsed();

        if (correct > 0 || incorrect > 0)
        {
            std::cout << "Speakers " << test.testGf
                      << ", accuracy "
                      << 100.0f * static_cast<float>(correct) / static_cast<float>(correct + incorrect) << "%" << std::endl;
        }

        results << GetLabel(test) << " " << correct << " " << incorrect << std::endl;

        std::ofstream perfTestFile(test.id + ".perftest", std::ios_base::app);
        perfTestFile << test.id + "_rec" + ".txt" << "|"
                     << GetLabel(test) << "|"
                     << realTestTime << "|"
                     << testData->GetSamples().size() << std::endl;
    }
}

void TestEngine::Recognize(
    const Test& test,
    std::shared_ptr<ModelRecognizer> recognizer)
{
    std::string resultsFileName = test.id + "_rec" + ".txt";

    std::ofstream results(resultsFileName, std::ios_base::app);

    auto testData = std::make_shared<SpeechData>();

    // Test utterances
    LoadTextSamples(test.features, testData, test.testSf, test.testGf, test.testSl, test.testGl, test.multiplier, false);

    unsigned int correct = 0;
    unsigned int incorrect = 0;

    // Select trained speakers.
    std::vector<SpeakerKey> speakers;

    for (unsigned int k = 0; k < test.testGf; ++k)
    {
        SpeakerKey key(GetSpeakerString(test.testSf + k, test.features));

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

    unsigned int sf = test.testSf;

    Real realTestTime = 0.0f;
    unsigned int realTestsTotal = 0;

    for (unsigned int i = 0; i < test.cycles ; i++)
    {
        std::cout << i + 1 << "/" << test.cycles << std::endl;

        // Test utterances
        LoadTextSamples(test.features, testData, sf, test.testGf, test.testSl, test.testGl, test.multiplier, false);

        Timer timer;
        for (const auto& samples : testData->GetSamples())
        {
            std::string speakerString;
            speakerString += samples.first.GetId()[0];
            speakerString += samples.first.GetId()[1];
            speakerString += samples.first.GetId()[2];

            if (recognizer->IsRecognized(SpeakerKey(speakerString), samples.second))
            {
                ++correct;
            }

            else
            {
                ++incorrect;
            }
        }
        realTestTime += timer.GetTimeElapsed();
        realTestsTotal += testData->GetSamples().size();

        sf += test.testGf;
    }

    results << GetLabel(test) << " " << correct << " " << incorrect << std::endl;

    std::ofstream perfTestFile(test.id + ".perftest", std::ios_base::app);
    perfTestFile << test.id + "_rec" + ".txt" << "|"
                 << GetLabel(test) << "|"
                 << realTestTime << "|"
                 << realTestsTotal << std::endl;
}

void TestEngine::Verify(
    const Test& test,
    std::shared_ptr<ModelRecognizer> recognizer)
{
    std::string resultsFileName = test.id + "_" + toString(test.index) + "_ver.txt";
    std::ofstream results(resultsFileName);

    // Select trained impostors.
    std::vector<SpeakerKey> impostors;
    for (unsigned int i = 0; i < test.gi; ++i)
    {
        SpeakerKey key(GetSpeakerString(test.si + i, test.features));

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

    unsigned int sf = test.testSf;

    unsigned int correctTrials = 0;
    unsigned int incorrectTrials = 0;

    Real realTestTime = 0.0f;

    for (unsigned int i = 0; i < test.cycles ; i++)
    {
        std::cout << i + 1 << "/" << test.cycles << std::endl;

        // Select trained speakers.
        std::vector<SpeakerKey> speakers;
        for (unsigned int k = 0; k < test.testGf; ++k)
        {
            SpeakerKey key(GetSpeakerString(sf + k, test.features));

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

        std::vector<Real> correctScores;
        std::vector<Real> incorrectScores;

        LoadTextSamples(test.features, testData, sf, test.testGf, test.testSl, test.testGl, test.multiplier, false);

        Timer timer;
        for (const auto& samples : testData->GetSamples())
        {
            std::string speakerString;
            speakerString += samples.first.GetId()[0];
            speakerString += samples.first.GetId()[1];
            speakerString += samples.first.GetId()[2];

            Real verificationResults = recognizer->GetVerificationScore(SpeakerKey(speakerString), samples.second);
            correctScores.push_back(verificationResults);
            ++correctTrials;

            for (const auto& imp : speakers)
            {
                if (imp != SpeakerKey(speakerString))
                {
                    Real verificationResults = recognizer->GetVerificationScore(SpeakerKey(imp), samples.second);
                    incorrectScores.push_back(verificationResults);
                    ++incorrectTrials;
                }
            }
        }
        realTestTime += timer.GetTimeElapsed();

        for (auto& entry : correctScores)
        {
            results << entry << " ";
        }

        results << std::endl;

        for (auto& entry : incorrectScores)
        {
            results << entry << " ";
        }

        results << std::endl;

        sf += test.testGf;
    }

    std::ofstream testFile(test.id + ".test", std::ios_base::app);

    testFile << resultsFileName << "|" << GetLabel(test) << std::endl;

    std::ofstream perfTestFile(test.id + ".perftest", std::ios_base::app);
    perfTestFile << test.id + "_" + toString(test.index) + "_ver.txt" << "|"
                 << GetLabel(test) << "|"
                 << realTestTime << "|"
                 << correctTrials << "|"
                 << incorrectTrials  << std::endl;
}

std::string TestEngine::GetLabel(const Test& test)
{
    if (test.label.empty())
    {
        return test.id + "_" + toString(test.index);
    }

    else
    {
        return test.label;
    }
}
