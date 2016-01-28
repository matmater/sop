#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG
    #define DEBUG_ASSERT(a) assert(a);
    #define DEBUG_ASSERT_MESSAGE(a, b) assert(a && b);

    #define DEBUG_CONTENT(a) a

    #if DEBUG_LEVEL >= 5
        #define DEBUG_FATAL(a) std::cout << "FATAL ERROR: " << a << std::endl; std::exit(EXIT_FAILURE);
        #define DEBUG_ERROR(a) std::cout << "ERROR: " << a << std::endl;
        #define DEBUG_WARNING(a) std::cout << "WARNING: " << a << std::endl;
        #define DEBUG_INFO(a) std::cout << "INFO: " << a << std::endl;
        #define DEBUG_TRACE(a) std::cout << "TRACE: " << a << std::endl;
    #elif DEBUG_LEVEL == 4
        #define DEBUG_FATAL(a) std::cout << "FATAL ERROR: " << a << std::endl; std::exit(EXIT_FAILURE);
        #define DEBUG_ERROR(a) std::cout << "ERROR: " << a << std::endl;
        #define DEBUG_WARNING(a) std::cout << "WARNING: " << a << std::endl;
        #define DEBUG_INFO(a) std::cout << "INFO: " << a << std::endl;
        #define DEBUG_TRACE(a)
    #elif DEBUG_LEVEL == 3
        #define DEBUG_FATAL(a) std::cout  << "FATAL ERROR: " << a << std::endl; std::exit(EXIT_FAILURE)
        #define DEBUG_ERROR(a) std::cout << "ERROR: " << a << std::endl;
        #define DEBUG_WARNING(a) std::cout << "WARNING: " << a << std::endl;
        #define DEBUG_INFO(a)
        #define DEBUG_TRACE(a)
    #elif DEBUG_LEVEL == 2
        #define DEBUG_FATAL(a) std::cout << "FATAL ERROR: " << a << std::endl; std::exit(EXIT_FAILURE)
        #define DEBUG_ERROR(a) std::cout << "ERROR: " << a << std::endl;
        #define DEBUG_WARNING(a)
        #define DEBUG_INFO(a)
        #define DEBUG_TRACE(a)
    #elif DEBUG_LEVEL == 1
        #define DEBUG_FATAL(a) std::cout << "FATAL ERROR: " << a << std::endl; std::exit(EXIT_FAILURE)
        #define DEBUG_ERROR(a)
        #define DEBUG_WARNING(a)
        #define DEBUG_INFO(a)
        #define DEBUG_TRACE(a)
    #else
        #define DEBUG_FATAL(a);
        #define DEBUG_INFO(a);
        #define DEBUG_WARNING(a);
        #define DEBUG_ERROR(a);
        #define DEBUG_TRACE(a);
    #endif
#else
    #define DEBUG_FATAL(a);
    #define DEBUG_INFO(a);
    #define DEBUG_WARNING(a);
    #define DEBUG_ERROR(a);
    #define DEBUG_TRACE(a);

    #define DEBUG_CONTENT

    #define DEBUG_ASSERT(a);
    #define DEBUG_ASSERT_MESSAGE(a, b);
#endif

#endif
