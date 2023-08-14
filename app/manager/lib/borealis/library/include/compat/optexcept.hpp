#ifndef OPTEXCEPT_HPP
#define OPTEXCEPT_HPP

#include <stdexcept>

// allow disabling exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND))
    #define TRY try
    #define CATCH(...) catch(__VA_ARGS__)
    #define FINALLY finally
#else
    #define TRY
    #define CATCH(...) if(0)
    #define FINALLY if(0)
#endif
#endif // OPTEXCEPT_HPP
