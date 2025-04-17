#include <stdio.h> /* perror() */

enum {FALSE , TRUE};


#ifndef NDEBUG
    #define DEBUG_ONLY(code) code
#else
    #define DEBUG_ONLY(code)
#endif


#ifndef NDEBUG
    #define ASSERT(condition)                                                                            \
        if (!(condition)) {                                                                              \
            fprintf(stderr, "ASSERT FAILED: %s, in file %s, line %d\n", #condition, __FILE__, __LINE__); \
            abort();                                                                                     \
        }
#else
    #define ASSERT(condition)
#endif



#define RETURN_IF_BAD(status, msg, return_status) \
        if ((status) != TRUE) {                   \
            perror(msg);                          \
            return (return_status);               \
        }


void ExitIfBad(int status, char* msg, int exit_status);
