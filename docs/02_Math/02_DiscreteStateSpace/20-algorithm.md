# 連続システムから離散システムへの変換 -実装編-

<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [連続システムから離散システムへの変換 -実装編-](#連続システムから離散システムへの変換--実装編-)
  - [0. 初めに](#0-初めに)
  - [1. 実装全体の流れ](#1-実装全体の流れ)
  - [2. 擬似コード](#2-擬似コード)

<!-- /code_chunk_output -->


---

## 0. 初めに
ZOH 入力の離散化では、$A_d = e^{Ah}, B_d = \int_0^h e^{A\tau} B \, d\tau$ を求める。
Van Loan の拡大行列 $M = [[A, B],[0, 0]]$ を用いれば、一度の行列指数 $e^{Mh}$ から $Ad$ と $Bd$ を同時に取り出せる。

---

## 1. 実装全体の流れ
離散化処理は`state_space_c2d()` で行われている。
関数内の基本的な流れは以下の通り：

|手順|関数例|処理内容|
|-  |    - |     - |
| 1. 拡大行列Mの構築| `matrix_core_create(), matrix_ops_set_zero(), matrix_ops_set_block()`| M を (n+m)×(n+m) で確保し、A、Bを各要素にセットする。その他はゼロに設定する |
| 2. Padé 近似  | `matrix_exp_exponential()`|行列指数関数exp(Mh)Pade近似で計算する|
| 3. ブロック抽出|`matrix_ops_get_block()`| E = exp(Mh) の左上ブロックを Ad、右上ブロックを Bd として取り出す|

---

## 2. 擬似コード

```pseudo
CoreErrorStatus state_space_c2d(const StateSpaceModel* sys, double Ts, Matrix* Ad, Matrix* Bd) {
	if (!sys || !sys->A || !sys->B || !Ad || !Bd)
		CORE_ERROR_RETURN(CORE_ERROR_NULL);

	const int n = sys->A->rows;
	const int m = sys->B->cols;

	if (sys->A->cols != n || sys->B->rows != n) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
	if (Ad->rows != n || Ad->cols != n) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
	if (Bd->rows != n || Bd->cols != m) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);

	CoreErrorStatus status;

	if (Ts == 0.0) {
		status = matrix_ops_set_identity(Ad); if (status) CORE_ERROR_RETURN(status);
		status = matrix_ops_set_zero(Bd); if (status) CORE_ERROR_RETURN(status);
		CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
	}

	Matrix* M = NULL, * E = NULL;
	
	// M: (n+m) x (n+m)
	M = matrix_core_create(n + m, n + m, &status);
	if (status) CORE_ERROR_RETURN(status);

	status = matrix_ops_set_zero(M);                                            if (status) goto FAIL;
	status = matrix_ops_set_block(M, 0, 0, sys->A);                      if (status) goto FAIL;
	status = matrix_ops_set_block(M, 0, sys->A->cols, sys->B);   if (status) goto FAIL;

	// M <- Ts * M
	status = matrix_ops_scale(M, Ts);                                             if (status) goto FAIL;

	// E = exp(M)
	E = matrix_core_create(M->rows, M->cols, &status);             if (status) goto FAIL;
	status = pade_expm(M, E);                                                      if (status) goto FAIL;

	// Ad = E(0:n-1, 0:n-1)
	status = matrix_ops_get_block(E, 0, 0, Ad);                              if (status) goto FAIL;

	// Bd = E(0:n-1, n:n+m-1)
	status = matrix_ops_get_block(E, 0, sys->A->cols, Bd);           if (status) goto FAIL;

	status = matrix_core_free(M);
	status = matrix_core_free(E);
	CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);

FAIL:
	if (E) matrix_core_free(E);
	if (M) matrix_core_free(M);
	CORE_ERROR_RETURN(status);
}
```

---
