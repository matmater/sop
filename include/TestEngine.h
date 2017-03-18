/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _TESTENGINE_H_
#define _TESTENGINE_H_

#include "Common.h"

#include "ModelRecognizer.h"

/*! \class TestEngine
 *  \brief An engine for VQ, GMM recognition & verification testing.
 */
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
        VERIFICATION,
        RECOGNITION_POPULATION
    };

    struct TestHeader
    {
        TestType type = TestType::UNKNOWN;
        std::string label = "";
        std::string targetId = "";
    };

    struct Test
    {
        unsigned int index = 0;

        std::string id = "";

        std::string label = "";

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

        unsigned int multiplier = 1;

        RecognizerType recognizerType = RecognizerType::UNKNOWN;
    };

public:
    /*! \brief Default constructor.
     */
    TestEngine();

    /*! \brief Run all tests in a file.
     */
    void Run(const std::string& file);

private:
    /*! \brief Varying population size recognition test.
     *
     *  \param test Test instructions.
     *  \param recognizer The recognizer to test.
     */
    void RecognizePop(
        const Test& test,
        std::shared_ptr<ModelRecognizer> recognizer);

    /*! \brief Basic speaker recognition.
     *
     *  \param test Test instructions.
     *  \param recognizer The recognizer to test.
     */
    void Recognize(
        const Test& test,
        std::shared_ptr<ModelRecognizer> recognizer);

    /*! \brief Basic speaker verification.
     *
     *  \param test Test instructions.
     *  \param recognizer The recognizer to test.
     */
    void Verify(
        const Test& test,
        std::shared_ptr<ModelRecognizer> recognizer);

private:
    /*! \brief Labels a test instruction.
     *
     *  \return A user specified test label (if defined) or
     *  an automatically generated label (otherwise).
     */
    std::string GetLabel(const Test& test);
};

#endif
