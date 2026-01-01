#pragma once
#include "runner_exit_codes.h"
#include "core_error.h"   // Lib‘¤‚ÌŒöŠJƒwƒbƒ_

using dts::runner::ExitCode;

ExitCode runner_exit_from_core_status(CoreErrorStatus st);
