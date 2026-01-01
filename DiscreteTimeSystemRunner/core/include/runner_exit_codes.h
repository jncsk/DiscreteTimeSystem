#pragma once

namespace dts::runner {
    enum class ExitCode : int {
        EXIT_OK = 0,
        EXIT_INVALID_INPUT = 2,
        EXIT_RUNTIME_ERROR = 3,
    };
}
