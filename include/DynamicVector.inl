template<typename T>
DynamicVector<T>::DynamicVector(unsigned int size)
{
    mValues.resize(size);
}

template<typename T>
DynamicVector<T>::~DynamicVector()
{

}
template<typename T>
DynamicVector<T>& DynamicVector<T>::operator= (const DynamicVector& other) const
{
    mValues.resize();
    Assign(other);

    return *this;
}

template<typename T>
void DynamicVector<T>::Resize(unsigned int size, T value)
{
    mValues.resize(size, value);
}

template<typename T>
unsigned int DynamicVector<T>::GetSize() const
{
    return mValues.size();
}

template<typename T>
void DynamicVector<T>::Push(T value)
{
    mValues.push_back(value);
}

template<typename T>
T& DynamicVector<T>::operator[] (unsigned int index)
{
    return mValues[index];
}

template<typename T>
const T& DynamicVector<T>::operator[] (unsigned int index) const
{
    return mValues[index];
}

template<typename T>
void DynamicVector<T>::Assign(T value)
{
    for (unsigned int i = 0; i < mValues.size(); i++)
    {
        mValues[i] = value;
    }
}

template<typename T>
void DynamicVector<T>::Assign(const DynamicVector& other)
{
    for (unsigned int i = 0; i < mValues.size(); i++)
    {
        mValues[i] = other[i];
    }
}

template<typename T>
void DynamicVector<T>::Multiply(T value)
{
    for (unsigned int i = 0; i < mValues.size(); i++)
    {
        mValues[i] *= value;
    }
}

template<typename T>
void DynamicVector<T>::Add(T value)
{
    for (unsigned int i = 0; i < mValues.size(); i++)
    {
        mValues[i] += value;
    }
}

template<typename T>
void DynamicVector<T>::Add(const DynamicVector& other)
{
    for (unsigned int i = 0; i < mValues.size(); i++)
    {
        mValues[i] += other.mValues[i];
    }
}

template<typename T>
void DynamicVector<T>::Subtract(const DynamicVector& other)
{
    for (unsigned int i = 0; i < mValues.size(); i++)
    {
        mValues[i] -= other.mValues[i];
    }
}

template<typename T>
T DynamicVector<T>::Distance(const DynamicVector& other) const
{
    T distance = T();

    for (unsigned int i = 0; i < mValues.size(); i++)
    {
        T diff = mValues[i] - other.mValues[i];

        distance += diff * diff;
    }

    return distance;
}
