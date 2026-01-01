#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "runner_exit_codes.h"
#include "runner_error_map.h"
#include "runner_io_json.h"

#include "core_matrix.h"
#include "core_error.h"
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
	if (sz <= 0) return static_cast<int>(ExitCode::EXIT_INVALID_INPUT);
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
	CoreErrorStatus* err = NULL;
	StateSpaceModel* sys = (StateSpaceModel*)calloc(1, sizeof(StateSpaceModel));
	if (!sys) {
		*err = CORE_ERROR_ALLOCATION_FAILED;
		CORE_ERROR_SET(*err);
		return NULL;
	}

	sys->A = matrix_core_create(req.A.rows, req.A.cols, err);
	sys->B = matrix_core_create(req.B.rows, req.B.cols, err);
	SSDiscrete d = { 0 };

	ss_discrete_init_from_csys(&d, sys, req.dt);

	// 5) result.json を書く（書けなければ exit 3）
	return (int)ExitCode::EXIT_OK;
}

