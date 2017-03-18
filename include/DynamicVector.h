/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _DYNAMICVECTOR_H_
#define _DYNAMICVECTOR_H_

#include "Common.h"

/*! \brief Multi-dimensional vector.
 *
 *  \tparam T Main data type of the values.
 */
template<typename T>
class DynamicVector
{
public:
    /*! \brief Constructor.
     *
     *  Allocates a defined number of elements of type T.
     *
     *  \param size The size of the vector (dimensions).
     */
    DynamicVector(unsigned int size = 0);

    /*! \brief Virtual destructor.
     */
    virtual ~DynamicVector();

    /*! \brief Copy assignment operator.
     *
     *  Copies all elements.
     */
    DynamicVector& operator= (const DynamicVector& other);

    /*! \brief Resize the vector and initialize new values to
     *  a default value.
     *
     *  \param size The size of the vector (dimensions).
     *  \param value The default value.
     */
    void Resize(unsigned int size, T value = T());

    /*! \brief Return the size of the vector (dimension count).
     *
     *  \return The size of the vector (dimension count).
     */
    unsigned int GetSize() const;

    /*! \brief Add a new value and increases the size of
     *  the vector by one.
     *
     *  \param value The value to be added.
     */
    void Push(T value);

    /*! \brief Non-const access operator.
     */
    T& operator[] (unsigned int index);

    /*! \brief Const access operator.
     */
    const T& operator[] (unsigned int index) const;

    /*! \brief Assign a given value to each dimension.
     *
     *  \param value The value to be assigned.
     */
    void Assign(T value);

    /*! \brief Multiply every dimension value by a given multiplier.
     *
     *  \param value The multiplier.
     */
    void Multiply(T value);

    /*! \brief Multiply by another vector dimension by dimension.
     *
     *  \param other The other vector.
     */
    void Multiply(const DynamicVector& other);

    /*! \brief Divide by another vector dimension by dimension.
     *
     *  \param other The other vector.
     */
    void Divide(const DynamicVector& other);

    /*! \brief Add a given value to each dimension.
     *
     *  \param value The summand.
     */
    void Add(T value);

    /*! \brief Vector copying.
     *
     *  Copies and assigns values from another vector.
     *
     *  \note Vector sizes must match.
     *
     *  \param other The other vector.
     */
    void Assign(const DynamicVector& other);

    /*! \brief Vector addition.
     *
     *  \param other The summand.
     */
    void Add(const DynamicVector& other);

    /*! \brief Vector subtraction.
     *
     *  \param other The subtrahend.
     */
    void Subtract(const DynamicVector& other);

    /*! \brief Calculate a squared euclidean distance to another vector.
     *
     *  \todo Change function name.
     *
     *  \param other The other vector.
     *
     *  \return The squared euclidean distance.
     */
    T Distance(const DynamicVector& other) const;

private:
    std::vector<T> mValues;
};

#include "DynamicVector.inl"

#endif
