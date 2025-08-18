# 連続システムから離散システムへの変換 -実装編-

<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [連続システムから離散システムへの変換 -実装編-](#連続システムから離散システムへの変換--実装編-)
  - [0. 初めに](#0-初めに)
  - [1. 実装全体の流れ](#1-実装全体の流れ)
  - [2. API 例（Control 層）](#2-api-例control-層)
  - [3. 手順詳細](#3-手順詳細)
  - [4. 擬似コード](#4-擬似コード)
  - [5. 数値安定化のヒント](#5-数値安定化のヒント)

<!-- /code_chunk_output -->


---

## 0. 初めに
ZOH 入力の離散化では、$A_d = e^{Ah}, B_d = \int_0^h e^{A\tau} B \, d\tau$ を求める。
Van Loan の拡大行列 $M = [[A, B],[0, 0]]$ を用いれば、一度の行列指数 $e^{Mh}$ から $Ad$ と $Bd$ を同時に取り出せる。

---

## 1. 実装全体の流れ
離散化処理は`state_space_c2d()` で行われている。
関数内の基本的な流れは以下の通り：

| 手順            | 関数例                                                                             | 処理内容                                                               |
| ------------- | ------------------------------------------------------------------------------- | ------------------------------------------------------------------ |
| 1. 拡大行列Mの構築    | `matrix_core_create(), matrix_ops_set_zero(), matrix_ops_set_block()`| M を (n+m)×(n+m) で確保し、A、Bを各要素にセットする。その他はゼロに設定する |
| 2. スカラー倍率     | `matrix_ops_scale()`| M を h 倍して Mh を得る（Expm に渡すのは Mh）|
| 3. Padé 近似  | `pade_expm()`|exp(Mh)に対してPade近似を計算する|
| 4. ブロック抽出|`matrix_ops_get_block()`| E = exp(Mh) の左上ブロックを Ad、右上ブロックを Bd として取り出す|

---

## 2. API 例（Control 層）

* state\_space\_c2d(const StateSpaceModel\* sys, double h, Matrix\* Ad, Matrix\* Bd)
  目的: A, B, h を入力に Ad, Bd を返す（ZOH、Van Loan 法）

内部で Numerics 層の行列演算と pade\_expm(M) を利用する。

---

## 3. 手順詳細

1. 拡大行列の構築

* M は (n+m)×(n+m)。
* ブロック配置:

  * M\[0\:n, 0\:n] = A
  * M\[0\:n, n\:n+m] = B
  * M\[n\:n+m, :] = 0 行
  * M\[:, n\:n+m] の対角は 0（右下は 0 行列）

2. スカラー倍率

* Mh = h \* M とし、これをそのまま pade\_expm に渡す。
  （pade\_expm 内でさらにスケーリング & スクエアリングが行われる）

3. 行列指数の計算（pade\_expm）

* 既存の Padé 実装をそのまま拡大行列に適用する。
* 内部で ノルム評価 → 次数/スケーリング選択 → V/U 構築 → LU 解法 → スクエアリング。

4. ブロック抽出

* E = exp(Mh) とすると、

  * Ad = E\[0\:n, 0\:n]
  * Bd = E\[0\:n, n\:n+m]
  * 参考: E\[n\:n+m, n\:n+m] は I（理論上）。数値では I からのズレを small なら無視。

5. 例外と高速化の注意

* h=0 のとき: Ad=I、Bd=0 を即時返却。
* A が十分に可逆かつ条件が良い場合は、Bd ≈ A^{-1}(Ad − I)B の式も使えるが、A が特異・準特異だと不安定のため基本は Van Loan 法を優先。

---

## 4. 擬似コード

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

## 5. 数値安定化のヒント

* 拡大行列のサイズは (n+m) になるため、メモリと計算量はその分増える。必要メモリを事前に確保し、再利用バッファを徹底。
* pade\_expm 内の LU 分解はピボット付き（部分ピボット）を用いる。
* 行列セットやスライス時に余計なコピーを避ける（ビュー/ブロック参照 API があれば活用）。
* 単体テストは以下を最低限用意：

  * B=0 のとき Bd=0、Ad=exp(Ah) に一致
  * A=0 のとき Ad=I、Bd=h\*B
  * 連続時間の安定 A に対して Ad の固有値が単位円内に収まる（小 h）

---

必要なら、上記をそのまま **state\_space\_c2d.c** の導入コメント（ドキュメント）に落とし込みます。さらに詳細な関数シグネチャやエラーコード設計（CoreErrorStatus 等）も合わせて書けます。
