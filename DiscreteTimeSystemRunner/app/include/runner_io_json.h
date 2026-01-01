#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "core_error.h"
#include "core_matrix.h"
#include "matrix_ops.h"

namespace dts::runner {

    using json = nlohmann::json;

    struct runner_matrix {
        int rows{};
        int cols{};
        std::vector<double> data;
    };

    inline void to_json(nlohmann::json& j, const runner_matrix& m) {
        j = { {"rows", m.rows}, {"cols", m.cols}, {"data", m.data} };
    }
    inline void from_json(const json& j, runner_matrix& m)
    {
        m.rows = j.at("rows").get<int>();
        m.cols = j.at("cols").get<int>();
        m.data = j.at("data").get<std::vector<double>>();

        if (m.rows <= 0 || m.cols <= 0)
            throw std::runtime_error("matrix rows/cols must be > 0");
        if ((int)m.data.size() != m.rows * m.cols)
            throw std::runtime_error("matrix data length mismatch");
    }

    struct discretize_request {
        std::string method;
        double dt{};
        runner_matrix A;
        runner_matrix B;
    };

    inline void from_json(const json& j, discretize_request& r)
    {
        r.method = j.at("method").get<std::string>();
        r.dt = j.at("dt").get<double>();

        // ‚±‚Á‚¿‚Í get<>() ‚æ‚è get_to() ‚Ì•û‚ª“Ç‚Ý‚â‚·‚¢
        j.at("A").get_to(r.A);
        j.at("B").get_to(r.B);

        if (!(r.dt > 0.0)) throw std::runtime_error("dt must be > 0");
        if (r.A.rows != r.A.cols) throw std::runtime_error("A must be square");
        if (r.B.rows != r.A.rows) throw std::runtime_error("B.rows must equal A.rows");
    }

} 

