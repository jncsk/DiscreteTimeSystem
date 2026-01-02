#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <optional>

#include <nlohmann/json.hpp>

#include "runner_exit_codes.h"
#include "runner_error_map.h"
#include "runner_io_json.h"

#include "core_matrix.h"
#include "core_error.h"
#include "matrix_ops.h"
#include "state_space.h"
#include "state_space_c2d.h"
#include "state_space_discrete.h"

using nlohmann::json;
using dts::runner::ExitCode;

#define MAX_BYTES 100000

static bool write_json_file(const std::string& out_path, const nlohmann::json& j) {
    std::ofstream os(out_path, std::ios::binary | std::ios::trunc);
    if (!os) return false;
    os << j.dump(2);
    os.flush();
    return (bool)os;
}

static json make_error_invalid_input(int exit_code, const char* msg) {
    json e = json::object();
    e["category"] = "invalid_input";
    e["exit_code"] = exit_code;
    e["core_status"] = json();          // JSON null
    e["message"] = std::string(msg);
    return e;
}

static json make_error_core(int exit_code, int core_status_int) {
    json e = json::object();
    e["category"] = "core_error";
    e["exit_code"] = exit_code;
    e["core_status"] = core_status_int;  // intでもOK（文字列化は後で）
    e["message"] = json();                     // JSON null
    return e;
}

static int write_result_or_runtime_error(
    const std::string& out_path,
    int code,
    bool ok,
    const json& ok_payload,
    const json& err_obj
) {
    json outj;
    outj["type"] = "discretize_result";
    outj["status"] = ok ? "succeeded" : "failed";
    outj["payload"] = ok ? ok_payload : json();
    outj["error"] = ok ? json() : err_obj;

    // out_path がある前提（argv parse 済み）
    if (!write_json_file(out_path, outj)) {
        return (int)ExitCode::EXIT_RUNTIME_ERROR;
    }
    return code;
}

// argv: DiscreteTimeSystemRunner.exe --in ../../../../testdata/input.json --out ../../../../testdata/result.json
int main(int argc, char** argv) {
    std::string in_path;
    std::string out_path;

    // argv parse（ここは out_path が無い可能性があるので早期return）
    for (int i = 1; i < argc; ++i) {
        std::string current_arg = argv[i];
        if (current_arg == "--in" && i + 1 < argc) {
            in_path = argv[++i];
        }
        else if (current_arg == "--out" && i + 1 < argc) {
            out_path = argv[++i];
        }
        else {
            return (int)ExitCode::EXIT_INVALID_INPUT;
        }
    }
    if (in_path.empty() || out_path.empty()) {
        return (int)ExitCode::EXIT_INVALID_INPUT;
    }

    // --- cleanup対象（goto を跨ぐ可能性があるので先に宣言） ---
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = nullptr;

    SSDiscrete d{};
    bool d_inited = false;

    // 入力まわり（gotoで初期化スキップされないよう先に宣言）
    std::filesystem::path p;
    std::uintmax_t sz = 0;
    std::ifstream stream;
    json root;

    std::optional<dts::runner::discretize_request> req_opt;

    // --- result.json 用 ---
    bool ok = false;
    json ok_payload = json::object();
    json err_obj = json(); // JSON null
    int exit_code = (int)ExitCode::EXIT_OK;

    // -------------------------
    // 入力ファイル検証
    // -------------------------
    p = std::filesystem::path(in_path);

    if (!std::filesystem::exists(p)) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "input file not found");
        goto CLEANUP;
    }
    if (!std::filesystem::is_regular_file(p)) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "input path is not a file");
        goto CLEANUP;
    }

    sz = std::filesystem::file_size(p);
    if (sz == 0) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "input file is empty");
        goto CLEANUP;
    }
    if (sz > MAX_BYTES) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "input file too large");
        goto CLEANUP;
    }

    stream.open(p, std::ios::binary);
    if (!stream) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "failed to open input file");
        goto CLEANUP;
    }

    try {
        root = json::parse(stream);
    }
    catch (...) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "invalid json");
        goto CLEANUP;
    }

    if (!root.contains("type") || !root["type"].is_string()) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "missing or invalid 'type'");
        goto CLEANUP;
    }
    if (root["type"].get<std::string>() != "discretize") {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "unsupported type");
        goto CLEANUP;
    }
    if (!root.contains("payload") || !root["payload"].is_object()) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "missing or invalid 'payload'");
        goto CLEANUP;
    }

    try {
        req_opt = root.at("payload").get<dts::runner::discretize_request>();
    }
    catch (...) {
        exit_code = (int)ExitCode::EXIT_INVALID_INPUT;
        err_obj = make_error_invalid_input(exit_code, "invalid payload schema");
        goto CLEANUP;
    }

    // -------------------------
    // 計算
    // -------------------------
    {
        // 以降、req_opt がある前提
        auto& req = *req_opt;

        sys = (StateSpaceModel*)calloc(1, sizeof(StateSpaceModel));
        if (!sys) {
            err = CORE_ERROR_ALLOCATION_FAILED;
            exit_code = (int)runner_exit_from_core_status(err);
            err_obj = make_error_core(exit_code, (int)err);
            goto CLEANUP;
        }

        sys->A = matrix_core_create(req.A.rows, req.A.cols, &err);
        if (err) {
            exit_code = (int)runner_exit_from_core_status(err);
            err_obj = make_error_core(exit_code, (int)err);
            goto CLEANUP;
        }
        for (int i = 0; i < req.A.rows; i++) {
            for (int j = 0; j < req.A.cols; j++) {
                err = matrix_ops_set(sys->A, i, j, req.A.data[i * req.A.cols + j]);
                if (err) {
                    exit_code = (int)runner_exit_from_core_status(err);
                    err_obj = make_error_core(exit_code, (int)err);
                    goto CLEANUP;
                }
            }
        }

        sys->B = matrix_core_create(req.B.rows, req.B.cols, &err);
        if (err) {
            exit_code = (int)runner_exit_from_core_status(err);
            err_obj = make_error_core(exit_code, (int)err);
            goto CLEANUP;
        }
        for (int i = 0; i < req.B.rows; i++) {
            for (int j = 0; j < req.B.cols; j++) {
                err = matrix_ops_set(sys->B, i, j, req.B.data[i * req.B.cols + j]);
                if (err) {
                    exit_code = (int)runner_exit_from_core_status(err);
                    err_obj = make_error_core(exit_code, (int)err);
                    goto CLEANUP;
                }
            }
        }

        err = ss_discrete_init_from_csys(&d, sys, req.dt);
        if (err) {
            exit_code = (int)runner_exit_from_core_status(err);
            err_obj = make_error_core(exit_code, (int)err);
            goto CLEANUP;
        }
        d_inited = true;

        // Ad/Bd を req に詰める（payload に載せるため）
        for (int i = 0; i < req.A.rows; i++) {
            for (int j = 0; j < req.A.cols; j++) {
                req.A.data[i * req.A.cols + j] = matrix_ops_get(d.Ad, i, j, &err);
                if (err) {
                    exit_code = (int)runner_exit_from_core_status(err);
                    err_obj = make_error_core(exit_code, (int)err);
                    goto CLEANUP;
                }
            }
        }
        for (int i = 0; i < req.B.rows; i++) {
            for (int j = 0; j < req.B.cols; j++) {
                req.B.data[i * req.B.cols + j] = matrix_ops_get(d.Bd, i, j, &err);
                if (err) {
                    exit_code = (int)runner_exit_from_core_status(err);
                    err_obj = make_error_core(exit_code, (int)err);
                    goto CLEANUP;
                }
            }
        }

        ok_payload = {
            {"method", req.method},
            {"dt", req.dt},
            {"Ad", req.A},
            {"Bd", req.B}
        };
        ok = true;
        exit_code = (int)ExitCode::EXIT_OK;
        err_obj = json(); // success: error = null
    }

CLEANUP:
    if (d_inited) {
        ss_discrete_free(&d);
    }
    if (sys) {
        state_space_free(sys);
    }

    return write_result_or_runtime_error(out_path, exit_code, ok, ok_payload, err_obj);
}
