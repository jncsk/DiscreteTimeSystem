#pragma once

/*
 * =============================================================================
 *  matrix.h
 * =============================================================================
 *
 *  Description:
 *      This header defines fundamental structures and core functions
 *      for matrix operations in C.
 *
 *  Features:
 *      - Creation and deletion of matrices
 *      - Matrix addition, subtraction, and multiplication
 *      - Matrix transposition
 *      - Scalar operations on matrices
 *
 * =============================================================================
 */

#include <stdlib.h>

//------------------------------------------------
//  Macro definitions
//------------------------------------------------
#define MATRIX_CORE_ERR_MESSAGE(code) \
    fprintf(stderr, "Error: process failed with error code %d (File: %s, Line: %d)\n", \
            (code), __FILE__, __LINE__)

#define MATRIX_CORE_SET_ERROR(errorcode) \
    do { \
        g_matrix_last_error.code = (errorcode); \
        g_matrix_last_error.file = __FILE__; \
        g_matrix_last_error.line = __LINE__; \
    } while(0)

#define RETURN_ERROR(errorcode) \
    do { \
        MATRIX_CORE_SET_ERROR(errorcode); \
        return errorcode; \
    } while (0)

#define MATRIX_CORE_PRINT_LAST_ERROR() \
    do { \
        MatrixError err = matrix_core_get_last_error(); \
        fprintf(stderr, "Error %d at %s:%d\n", err.code, err.file, err.line); \
    } while (0)

#if defined(_MSC_VER)
    #define THREAD_LOCAL __declspec(thread)
#else
    #define THREAD_LOCAL __thread
#endif
//------------------------------------------------
//  Type definitions
//------------------------------------------------
typedef enum {
    MATRIX_CORE_SUCCESS = 0,
    MATRIX_CORE_ERR_NULL = -1,
    MATRIX_CORE_ERR_DIMENSION = -2,
    MATRIX_CORE_ERR_OUT_OF_BOUNDS = -3,
    MATRIX_CORE_ERR_ALLOCATION_FAILED = -4,
} MatrixCoreStatus;

typedef struct {
    int code;
    const char* file;
    int line;
} MatrixError;

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
 * @param err Error code.
 * @return A Matrix structure with allocated memory.
 */
Matrix* matrix_create(int rows, int cols, int* err);

/**
 * @brief Free the memory associated with a matrix.
 *
 * @param mat Pointer to the matrix to free.
 */
int matrix_free(Matrix* mat);

MatrixError matrix_core_get_last_error(void);
