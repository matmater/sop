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
 