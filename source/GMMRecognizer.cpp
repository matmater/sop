/*!
 *  This file is part of a speaker recognition group project.
 *
 *  \author Markus Nykänen <mnykne@gmail.com>
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
