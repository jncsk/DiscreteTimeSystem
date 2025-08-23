// app_motor.h
#pragma once
#include "state_space.h"

typedef struct {
    double J;   // 慣性モーメント
    double D;   // 粘性摩擦
    double M;   // モータ定数
    double R;   // 抵抗
    double K;   //パワーアンプの電圧増幅率
} DCMotorParams;

/**
 * @brief Create a continuous-time state-space model of a DC servo motor.
 *
 * @param[in]  p   Motor parameters
 * @param[out] err Error code
 * @return StateSpaceModel* (caller must free with state_space_free)
 */
DCMotorParams* motor_set_params(double J, double D, double M, double R, double K);

/**
 * @brief Create a continuous-time state-space model of a DC servo motor.
 *
 * @param[in]  p   Motor parameters
 * @param[out] err Error code
 * @return StateSpaceModel* (caller must free with state_space_free)
 */
StateSpaceModel* motor_create(const DCMotorParams* p, CoreErrorStatus* err);

