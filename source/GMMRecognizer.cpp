#include "GMMRecognizer.h"
#include "GMModel.h"

GMMRecognizer::GMMRecognizer()
{

}

GMMRecognizer::~GMMRecognizer()
{

}

void GMMRecognizer::Train()
{
    ModelRecognizer::Train();
}

void GMMRecognizer::Test(const std::shared_ptr<SpeechData>& data, std::map<SpeakerKey, RecognitionResult>& results)
{
    ModelRecognizer::Test(data, results);
}

std::shared_ptr<Model> GMMRecognizer::CreateModel()
{
    return std::make_shared<GMModel>();
}
