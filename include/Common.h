#ifndef _COMMON_H_
#define _COMMON_H_

#include <math.h>
#include <iostream>
#include <math.h>
#include <random>
#include <cassert>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>
#include <iomanip>

#define DEBUG
#define DEBUG_LEVEL 5
#include "Debug.h"

#define PI_F 3.14159265f

typedef double Real;

/*! \brief Returns the sign of a given value.
 *
 *  \param value The value to be examined.
 *  \return The sign of the given value as an integer: -1, 0, or 1.
 */
template<typename T>
int Sign(const T& value)
{
    return (T(0) < value) - (value < T(0));
}


/*! \brief Converts a given string to a type T.
 *
 *  Conversion is done by using std::stringstream.
 *
 *  \param value The string to be converted.
 *  \return The converted value of type T.
 */
template<typename T>
inline T ConvertString(const std::string& str)
{
    T v;
    std::stringstream ss(str);
    ss >> v;
    return v;
}

/*! \brief Returns the smallest value of the two given values.
 *
 *  \param a The first value to be compared.
 *  \param b The second value to be compared.
 *  \return The smallest value of the two given values.
 */
template<typename T>
const T& Min(const T& a, const T& b)
{
    return (a < b) ? a : b;
}

/*! \brief Returns the largest value of the two given values.
 *
 *  \param a The first value to be compared.
 *  \param b The second value to be compared.
 *  \return The largest value of the two given values.
 */
template<typename T>
const T& Max(const T& a, const T& b)
{
    return (a > b) ? a : b;
}

/*! \brief Generates a random value between 0.0 and 1.0.
 *
 *  \return A random value [0.0, 1.0].
 */
inline Real Random()
{
    return rand() / static_cast<Real>(RAND_MAX);
}

/*! \brief Generates a random value between given boundaries.
 *
 *  \param min The lower bound of the value.
 *  \param max The upper bound of the value.
 *  \return A random value [min, max].
 */
inline Real Random(Real min, Real max)
{
    return Random() * (max - min) + min;
}

/*! \brief Clamps a value between requested boundaries.
 *
 *  \param min The lower bound of the value.
 *  \param max The upper bound of the value.
 *  \param value The value to be clamped.
 *  \return The clamped value.
 */
template<typename T>
const T& Clamp(const T& min, const T& max, const T& value)
{
    return Max(min, Min(max, value));
}

/*! \brief The default activation function.
 *
 *  \note This function is used for debugging.
 *  \todo Maybe switch/case structure to support multiple functions.
 */
inline Real Sigmoid(Real v)
{
    return std::tanh(v);
}

/*! \brief The derivative of the default activation function.
 *
 *  \warning This is currently used for debugging so specs may differ.
 *
 *  \todo maybe switch/case structure to support multiple functions.
 */
inline Real SigmoidDelta(Real v)
{
    // Avoid zero situation to prevent getting stuck.
    // A Direct Adaptive Method for Faster Backpropagation Learning: The RPROP Algorithm
    // http://deeplearning.cs.cmu.edu/pdfs/Rprop.pdf

    if (v < -0.98f || v > 0.98f)
    {
        // Just a precomputed value: 1 - 0.97^2.
        return 0.0591f;
    }

    // Derivative of tanh, assuming v = tanh(x).
    return 1.0f - v * v;
}

/*! \brief Generates a gaussian random value in the range [-1.0f, 1.0f].
 *
 *  \algorithm Box-Muller transform (polar form). http://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform.
 */
inline Real RandomGaussian()
{
    Real u, v, s;

    do
    {
        u = Random(-1.0f, 1.0f);
        v = Random(-1.0f, 1.0f);
        s = u * u + v * v;

    } while (s >= 1.0f || s == 0.0f);

    return (u * std::sqrt((-2.0f * std::log(s)) / s));
}

#endif
