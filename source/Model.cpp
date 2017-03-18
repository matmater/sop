/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#include "Model.h"

Model::Model()
: mOrder(128)
{

}

Model::~Model()
{

}

void Model::SetOrder(unsigned int order)
{
    mOrder = order;
}

unsigned int Model::GetOrder() const
{
    return mOrder;
}
