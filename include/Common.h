/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#define NDEBUG

#include <math.h>
#include <iostream>
#include <math.h>
#include <random>
#include <cassert>
#include <algorithm>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>
#include <iomanip>
#include <memory>
#include <vector>
#include <iomanip>
#include <functional>

#define INDEXFIX
//#undef INDEXFIX

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

/*! \brief Generates a random value in range 0.0 and 1.0.
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
    // A Direct Adaptive Method for Faster Backpropagation Learning:
    // The RPROP Algorithm http://deeplearning.cs.cmu.edu/pdfs/Rprop.pdf

    if (v < -0.98f || v > 0.98f) {
        // Just a precomputed value: 1 - 0.97^2.
        return 0.0591f;
    }

    // Derivative of tanh, assuming v = tanh(x).
    return 1.0f - v * v;
}

/*! \brief Generates a gaussian random value in range [-1.0f, 1.0f].
 *
 *  \algorithm Box-Muller transform (polar form).
 *  http://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform.
 */
inline Real RandomGaussian()
{
    Real u, v, s;

    do {
        u = Random(-1.0f, 1.0f);
        v = Random(-1.0f, 1.0f);
        s = u * u + v * v;
    } while (s >= 1.0f || s == 0.0f);

    return (u * std::sqrt((-2.0f * std::log(s)) / s));
}

/*! \brief Calculates the mean of the values.
 *
 *  Equation: Sum(values)/Size(values)
 */
inline Real Mean(const std::vector<Real>& values)
{
    if (values.size() == 0) {
        std::cout << "Mean could not be calculated: no values." << std::endl;
        return 0.0f;
    }

    Real sum = 0.0f;
    Real invSize = 1.0f / static_cast<float>(values.size());

    for (Real value : values) {
        sum += value * invSize;
    }

    return sum;
}

/*! \brief Returns the variance of the values with a given mean value.
 *
 *  Equation: Sum(values)/(Size(values) - 1)
 */
inline Real Variance(const std::vector<Real>& values, Real mean)
{
    if (values.size() < 2) {
        std::cout << "Variance could not be calculated: not enough values."
                  << std::endl;

        return 0.0f;
    }

    Real sum = 0.0f;
    Real invSize = 1.0f / static_cast<float>(values.size() - 1);

    for (Real value : values) {
        Real tmp = value - mean;
        sum += tmp * tmp * invSize;
    }

    return sum;
}

/*! \brief Returns the standard deviation of the
 *  values with the given mean value.
 *
 *  Equation: sqrt[sum(values)/(size(values) - 1)]
 */
inline Real Deviation(const std::vector<Real>& values, Real mean)
{
    return std::sqrt(Variance(values, mean));
}

inline bool FileExists(const std::string& path)
{
    std::ifstream file(path);

    return file.good();
}

template <typename T>
std::string toString(const T& n)
{
    std::ostringstream s;
    s << n;
    return s.str();
}

inline bool GetStringLiteral(std::istream& stream, std::string& str)
{
    char c;

    std::streampos oldPos = stream.tellg();

    std::string result;

    if (!((stream >> c) && c == '"')) {
        stream.clear();
        stream.seekg(oldPos);
        return false;
    }

    if (!std::getline(stream, result, '"')) {
        stream.clear();
        stream.seekg(oldPos);
        return false;
    }

    str = result;

    return true;
}

#endif
