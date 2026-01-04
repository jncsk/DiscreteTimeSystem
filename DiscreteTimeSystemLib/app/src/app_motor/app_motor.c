// app_motor.c
#include "app_motor.h"
#include "matrix_ops.h"
#include <math.h>

DCMotorParams* motor_set_params(double J, double D, double M, double R, double K) {

    DCMotorParams* p = (DCMotorParams*) malloc(sizeof(DCMotorParams));

    p->J = J;
    p->D = D;
    p->M = M;
    p->R = R;
    p->K = K;

    return p;
}


StateSpaceModel* motor_create(const DCMotorParams* p, CoreErrorStatus* err) {
    StateSpaceModel* sys = state_space_create(2, 1, 1, err); // 3状態, 1入力, 1出力
    if (!sys) return NULL;

    // A行列
    // [    1      0  ]
    // [ -b/J   K/J  ]
    matrix_ops_set(sys->A, 0, 0, 0.0);
    matrix_ops_set(sys->A, 0, 1, 1.0);

    matrix_ops_set(sys->A, 1, 0, 0.0);
    matrix_ops_set(sys->A, 1, 1, -(p->D / p->J + pow(p->M) / (p->J * p->R)));

    // B行列
    matrix_ops_set(sys->B, 0, 0, 0.0);
    matrix_ops_set(sys->B, 1, 0, (p->M * p->K) / (p->R * p->J));

    // C, D はとりあえず単位行列 or ゼロ
    // 例: 出力を角速度ωにしたい場合
    matrix_ops_set(sys->C, 0, 0, 0.0);
    matrix_ops_set(sys->C, 0, 1, 1.0);
    matrix_ops_set(sys->C, 0, 2, 0.0);

    return sys;
}
