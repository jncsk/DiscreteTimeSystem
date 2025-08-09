#pragma once

/*
 * =============================================================================
 *  matrix_core.h
 * =============================================================================
 *
 *  Description:
 *      This header defines fundamental structures, macros, and core functions
 *      for matrix operations in C.
 *
 *  Features:
 *      - Creation and deletion of matrices
 *      - Matrix addition, subtraction, and multiplication
 *      - Matrix transposition
 *      - Scalar operations on matrices
 *      - Centralized error handling and reporting
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
#define MATRIX_CORE_PRINT_CALL_AND_LAST(errorcode) \
    do { \
        fprintf(stderr, "[Caller] Error %d at %s:%d\n", (errorcode), __FILE__, __LINE__); \
        fprintf(stderr, "[Origin] Error %d at %s:%d\n", \
                g_matrix_last_error.code, \
                g_matrix_last_error.file, \
                g_matrix_last_error.line); \
    } while (0)

 /**
  * @brief Print error details if the status indicates failure.
  *
  * @param status MatrixCoreStatus value to check.
  */
#define MATRIX_CORE_HANDLE_ERR(status) \
    do { \
        if ((status) != MATRIX_CORE_SUCCESS) { \
            MATRIX_CORE_PRINT_CALL_AND_LAST(status); \
        } \
    } while (0)

  /**
   * @brief Set the last error details (code, file, and line).
   *
   * @param errorcode Error code to set.
   */
#define MATRIX_CORE_SET_ERROR(errorcode) \
    do { \
        g_matrix_last_error.code = (errorcode); \
        g_matrix_last_error.file = __FILE__; \
        g_matrix_last_error.line = __LINE__; \
    } while(0)

   /**
    * @brief Set the last error details and immediately return the error code.
    *
    * @param errorcode Error code to set and return.
    */
#define RETURN_ERROR(errorcode) \
    do { \
        MATRIX_CORE_SET_ERROR(errorcode); \
        return (errorcode); \
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
    MATRIX_CORE_SUCCESS = 0,
    MATRIX_CORE_ERR_NULL = -1,
    MATRIX_CORE_ERR_DIMENSION = -2,
    MATRIX_CORE_ERR_OUT_OF_BOUNDS = -3,
    MATRIX_CORE_ERR_ALLOCATION_FAILED = -4,
} MatrixCoreStatus;

/**
 * @brief Structure for storing details of the last error.
 */
typedef struct {
    int code;
    const char* file;
    int line;
} MatrixError;

/**
 * @brief Thread-local storage for last error information.
 */
extern THREAD_LOCAL MatrixError g_matrix_last_error;

/**
 * @brief Structure representing a 2D matrix.
 *
 * The matrix is stored in row-major order as a 1D array:
 * element at (i, j) is located at data[i * cols + j].
 */
typedef struct {
    int rows;
    int cols;
    double* data;  // Row-major: data[i * cols + j]
} Matrix;

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Create a new matrix with specified dimensions.
 *
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @param err Pointer to MatrixCoreStatus for storing error code (can be NULL).
 * @return Pointer to allocated Matrix, or NULL on failure.
 */
Matrix* matrix_core_create(int rows, int cols, MatrixCoreStatus* err);

/**
 * @brief Create a new square matrix with specified dimensions.
 *
 * @param size Number of rows/columns.
 * @param err Pointer to MatrixCoreStatus for storing error code (can be NULL).
 * @return Pointer to allocated Matrix, or NULL on failure.
 */
Matrix* matrix_core_create_square(int size, MatrixCoreStatus* err);

/**
 * @brief Free the memory associated with a matrix.
 *
 * @param mat Pointer to the matrix to free.
 * @return MATRIX_CORE_SUCCESS if freed successfully, otherwise an error code.
 */
MatrixCoreStatus matrix_core_free(Matrix* mat);

/**
 * @brief Get the last error that occurred in matrix operations.
 *
 * @return MatrixError structure containing the last error's details.
 */
MatrixError matrix_core_get_last_error(void);
