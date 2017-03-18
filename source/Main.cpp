/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>

#include "Common.h"

#include "VQRecognizer.h"
#include "GMMRecognizer.h"

#include "TestEngine.h"

int main(int argc, char** argv)
{
    // !!!!!!!!
    // INDEXFIX macro in Common.h !
    // !!!!!!!!

    /* See tests folder for examples. */

    //std::freopen("output.txt", "w", stdout);

    TestEngine engine;

    if (argc >= 2)
    {
        engine.Run(argv[1]);
    }

    else
    {
        engine.Run("tests.txt");
    }

    system("pause");
}
