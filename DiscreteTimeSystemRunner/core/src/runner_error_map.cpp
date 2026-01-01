#include "runner_error_map.h"

ExitCode runner_exit_from_core_status(CoreErrorStatus st)
{
    if (st == CORE_ERROR_SUCCESS) return ExitCode::EXIT_OK;

    switch (st) {
    case CORE_ERROR_NULL:
    case CORE_ERROR_DIMENSION:
    case CORE_ERROR_OUT_OF_BOUNDS:
    case CORE_ERROR_INVALID_ARG:
        return ExitCode::EXIT_INVALID_INPUT;

    case CORE_ERROR_ALLOCATION_FAILED:
    case CORE_ERROR_NOMEM:
    case CORE_ERROR_NUMERIC:
    default:
        return ExitCode::EXIT_RUNTIME_ERROR;
    }
}
