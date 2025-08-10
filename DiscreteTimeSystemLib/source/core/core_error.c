#pragma once
#include <stdio.h>
#include "matrix_core.h"

THREAD_LOCAL CoreError g_last_error = { 0, NULL, 0 };

CoreError matrix_core_get_last_error(void) {
    return g_last_error;
}
