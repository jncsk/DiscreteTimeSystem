#pragma once

/*
 * =============================================================================
 *  core_error.h
 * =============================================================================
 *
 *  Description:
 *      Centralized error handling utilities for core modules in C.
 *
 *  Features:
 *      - Unified error code definitions
 *      - Macros for setting and returning errors
 *      - Last error tracking (code, file, and line)
 *
 * =============================================================================
 */

#include <stdlib.h>
#include <stdio.h> // For fprintf in macros

 //------------------------------------------------
 //  Macro definitions
 //------------------------------------------------

 /**
  * @brief Print both the caller's location and the original error source.
  *
  * @param errorcode Error code to display for the caller location.
  */
#define CORE_ERROR_PRINT_CALL_AND_LAST(errorcode) \
    do { \
        fprintf(stderr, "[Caller] Error %d at %s:%d\n", (errorcode), __FILE__, __LINE__); \
        fprintf(stderr, "[Origin] Error %d at %s:%d\n", \
                g_last_error.code, \
                g_last_error.file, \
                g_last_error.line); \
    } while (0)

/**
 * @brief Set the last error details (code, file, and line).
 *
 * @param errorcode Error code to set.
*/
#define CORE_ERROR_SET(errorcode) \
    do { \
        g_last_error.code = (errorcode); \
        g_last_error.file = __FILE__; \
        g_last_error.line = __LINE__; \
    } while(0)

/**
 * @brief Set the last error details and immediately return the error code.
 *
 * @param errorcode Error code to set and return.
*/
//#define CORE_ERROR_RETURN(errorcode) \
//    do { \
//        if(errorcode == CORE_ERROR_SUCCESS){ \
//            return (errorcode); \
//        } \
//     else { \
//        CORE_ERROR_SET(errorcode); \
//        return (errorcode); \
//      } \
//    } while (0)

#define CORE_ERROR_RETURN(errorcode) \
    do { \
        CoreErrorStatus __e = (errorcode); \
        if (__e != CORE_ERROR_SUCCESS) { \
            CORE_ERROR_SET(__e); \
        } \
        return __e; \
    } while (0)

#if defined(_MSC_VER)
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL __thread
#endif

//------------------------------------------------
//  Type definitions
//------------------------------------------------

/**
 * @brief Enumeration of core matrix status/error codes.
*/
typedef enum {
    CORE_ERROR_SUCCESS = 0,
    CORE_ERROR_NULL = -1,
    CORE_ERROR_DIMENSION = -2,
    CORE_ERROR_OUT_OF_BOUNDS = -3,
    CORE_ERROR_ALLOCATION_FAILED = -4,
    CORE_ERROR_INVALID_ARG = -5,
    CORE_ERROR_NOMEM = -6,
    CORE_ERROR_NUMERIC = -7,
} CoreErrorStatus;

/**
 * @brief Structure for storing details of the last error.
 */
typedef struct {
    int code;
    const char* file;
    int line;
} CoreError;

/**
 * @brief Thread-local storage for last error information.
 */
extern THREAD_LOCAL CoreError g_last_error;

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Get the last error that occurred in matrix operations.
 *
 * @return MatrixError structure containing the last error's details.
 */
CoreError matrix_core_get_last_error(void);

