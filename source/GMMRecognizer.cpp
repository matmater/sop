/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#include "GMMRecognizer.h"
#include "GMModel.h"

GMMRecognizer::GMMRecognizer()
{

}

GMMRecognizer::~GMMRecognizer()
{

}

std::shared_ptr<Model> GMMRecognizer::CreateModel()
{
    return std::make_shared<GMModel>();
}
