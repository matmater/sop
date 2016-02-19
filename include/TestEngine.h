#ifndef _TESTENGINE_H_
#define _TESTENGINE_H_

#include "Common.h"

#include "ModelRecognizer.h"

class TestEngine
{
private:
    enum class RecognizerType
    {
        UNKNOWN = 0,
        VQ,
        GMM
    };

    enum class TestType
    {
        UNKNOWN = 0,
        RECOGNITION,
        VERIFICATION
    };

    struct Test
    {
        std::string id = "";

        std::string features = "";
        
        bool weighting = false;
        bool ubm = false;
        ScoreNormalizationType scoreNormalizationType = ScoreNormalizationType::NONE;
        unsigned int order = 1;

        TestType type = TestType::UNKNOWN;
        
        unsigned int trainSf = 0;
        unsigned int trainGf = 0;
        unsigned int trainSl = 0;
        unsigned int trainGl = 0;

        unsigned int testSf = 0;
        unsigned int testGf = 0;
        unsigned int testSl = 0;
        unsigned int testGl = 0;
        unsigned int cycles = 0;
        unsigned int incorrectClaimed = 0;
        unsigned int correctClaimed = 0;
        unsigned int si = 0;
        unsigned int gi = 0;

        RecognizerType recognizerType = RecognizerType::UNKNOWN;
    };

    struct Bundle
    {
        std::map < std::string, std::vector< std::shared_ptr<ModelRecognizer> > > tests;
    };

public:
    TestEngine();

    void Run(const std::string& file);

    void Recognize(
        const std::string& id,
        const std::string& features,
        std::shared_ptr<ModelRecognizer> recognizer,
        unsigned int sf,
        unsigned int gf,
        unsigned int sl,
        unsigned int gl,
        unsigned int cycles);

    void Verify(
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
        unsigned int gi);

private:
    std::string GetIdentifier(std::shared_ptr<ModelRecognizer> recognizer);
};

#endif
