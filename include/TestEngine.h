#ifndef _TESTENGINE_H_
#define _TESTENGINE_H_

#include "Common.h"

class ModelRecognizer;
class TestBundle;

class TestEngine
{
public:
    TestEngine();

    void Recognize(
        const std::string& id,
        std::shared_ptr<ModelRecognizer> recognizer,
        unsigned int sf,
        unsigned int gf,
        unsigned int sl,
        unsigned int gl,
        unsigned int cycles);

    void Verify(
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
        unsigned int gi);

private:
    std::string GetIdentifier(std::shared_ptr<ModelRecognizer> recognizer);
};

#endif
