#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
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

// argv: DiscreteTimeSystemRunner.exe --in input.json --out result.json
int main(int argc, char** argv) {

	std::string in_path;
	std::string out_path;

	// Retrieve "--in" and "--out" from argv
	for (int i = 1; i < argc; ++i) {
		std::string current_arg = argv[i];
		if (current_arg == "--in" && i + 1 < argc) {
			in_path = argv[++i];
		}
		else if (current_arg == "--out" && i + 1 < argc) {
			out_path = argv[++i];
		}
		else {
			return static_cast<int>(ExitCode::EXIT_INVALID_INPUT);
		}
	}
	// Empty Check
	if (in_path.empty() || out_path.empty()) {
		return static_cast<int>(ExitCode::EXIT_INVALID_INPUT);
	}
	// File Check
	std::filesystem::path p(in_path);
	if (!std::filesystem::exists(p)) {
		return static_cast<int>(ExitCode::EXIT_INVALID_INPUT);
	}
	if (!std::filesystem::is_regular_file(p)) {
		return static_cast<int>(ExitCode::EXIT_INVALID_INPUT);
	}
	auto sz = std::filesystem::file_size(p);
	if (sz == 0) return (int)ExitCode::EXIT_INVALID_INPUT;
	if (sz > MAX_BYTES) return static_cast<int>(ExitCode::EXIT_INVALID_INPUT);

	// 3) A,B,dt を取り出す（次元不整合なら exit 2）
	// パス p のファイルを入力ストリームとして開く。
	//	std::ios::binary
	// Windows で改行変換などをしないように「バイナリ扱い」で読む指定（JSON でも付けても問題ない）。
	std::ifstream stream(p, std::ios::binary);
	if (!stream) return (int)ExitCode::EXIT_INVALID_INPUT;

	nlohmann::json root;
	try {
		root = nlohmann::json::parse(stream);
	}
	catch (...) {
		return (int)ExitCode::EXIT_INVALID_INPUT;
	}

	if (root.at("type").get<std::string>() != "discretize") {
		return (int)ExitCode::EXIT_INVALID_INPUT;
	}

	dts::runner::discretize_request req;
	try {
		req = root.at("payload").get<dts::runner::discretize_request>();
	}
	catch (...) {
		return (int)ExitCode::EXIT_INVALID_INPUT;
	}

	// 4) 計算（失敗したら exit 3）
	CoreErrorStatus err = CORE_ERROR_SUCCESS;
	StateSpaceModel* sys = (StateSpaceModel*)calloc(1, sizeof(StateSpaceModel));
	if (!sys) {
		err = CORE_ERROR_ALLOCATION_FAILED;
		if (err) return (int)runner_exit_from_core_status(err);
	}

	sys->A = matrix_core_create(req.A.rows, req.A.cols, &err);
	if (err) return (int)runner_exit_from_core_status(err);
	for (int i = 0; i < req.A.rows; i++) {
		for (int j = 0; j < req.A.cols; j++) {
			err = matrix_ops_set(sys->A, i, j, req.A.data[i * req.A.cols + j]);
			if (err) return (int)runner_exit_from_core_status(err);
		}
	}

	sys->B = matrix_core_create(req.B.rows, req.B.cols, &err);
	if (err) return (int)runner_exit_from_core_status(err);
	for (int i = 0; i < req.B.rows; i++) {
		for (int j = 0; j < req.B.cols; j++) {
			err = matrix_ops_set(sys->B, i, j, req.B.data[i * req.B.cols + j]);
			if (err) return (int)runner_exit_from_core_status(err);
		}
	}

	SSDiscrete d = { 0 };

	err = ss_discrete_init_from_csys(&d, sys, req.dt);
	if (err) return (int)runner_exit_from_core_status(err);

	// 5) result.json を書く（書けなければ exit 3）
	for (int i = 0; i < req.A.rows; i++) {
		for (int j = 0; j < req.A.cols; j++) {
			req.A.data[i * req.A.cols + j] = matrix_ops_get(d.Ad, i, j, &err);
			if (err) return (int)runner_exit_from_core_status(err);
		}
	}

	for (int i = 0; i < req.B.rows; i++) {
		for (int j = 0; j < req.B.cols; j++) {
			req.B.data[i * req.B.cols + j] = matrix_ops_get(d.Bd, i, j, &err);
			if (err) return (int)runner_exit_from_core_status(err);
		}
	}

	// 出力JSONを組み立て
	nlohmann::json payload;
	payload["method"] = req.method;
	payload["dt"] = req.dt;
	payload["Ad"] = req.A;
	payload["Bd"] = req.B;

	nlohmann::json out;
	out["type"] = "discretize_result";
	out["payload"] = std::move(payload);

	// ファイルに書く
	std::ofstream os(out_path, std::ios::binary | std::ios::trunc);
	if (!os) return (int)ExitCode::EXIT_RUNTIME_ERROR;

	os << out.dump(2);
	os.flush();
	if (!os) return (int)ExitCode::EXIT_RUNTIME_ERROR;

	err = state_space_free(sys);
	if (err) return (int)runner_exit_from_core_status(err);

	err = ss_discrete_free(&d);
	if (err) return (int)runner_exit_from_core_status(err);

	return (int)ExitCode::EXIT_OK;
}

