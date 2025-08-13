いいね、その2つは必須だけど、**それだけだと足りない**。
行列指数 $e^{A t}$ を **Scaling & Squaring + パデ**で実装するなら、最低限これだけ要ります（小さく作る順番で並べたチェックリスト）。

# 実装チェックリスト（最小構成）

1. **行列ノルム**

* 目的: スケーリング量 $s$ と次数 $m$ を決める。
* 推奨: 1-ノルム（列和）か無限大ノルム（行和）で十分。
* API例: `double mat_norm1(const double* A, int n);`

2. **パデ係数（固定）**

* 目的: 毎回連立を解かずに高速・安定に。
* 内容: $[m/m]$ 用の係数テーブル（例：m=3,5,7,9,13）。
  ※exp 専用の既知係数を配列で持つ。

3. **次数とスケーリングの決定**

* 目的: $\|A t / 2^s\|$ を閾値 $\theta_m$ 以下に。
* すること: ノルムから **$s$** を選ぶ → 係数表から **$m$** を選ぶ。
* API例: `void choose_scale_and_order(double anorm, int* s, int* m);`
  （$\theta_m$ は係数とセットでテーブル化）

4. **行列の冪（必要分だけ）**

* 目的: $U,V$ を作るための $A^2, A^4, A^6, …$ を再利用。
* API例: `matmul(A,B,C)` / `copy`, `axpy`, など最小限。

5. **U と V の構成**

* 目的: $R_{m,m}(A_s)= (V-U)^{-1}(V+U)$ の **U（奇）/V（偶）** 多項式を作る。
* 例:
  $U = c_1 A + c_3 A^3 + \cdots,\quad V = I + c_2 A^2 + c_4 A^4 + \cdots$
* API例: `void form_UV(const double* A, const Powers* P, int m, Mat* U, Mat* V);`

6. **線形方程式を解く（逆行列は作らない）**

* 目的: $(V-U)X = (V+U)$ を解いて $X=R_{m,m}(A_s)$ を得る。
* 推奨: LU（部分ピボット）`dgesv` 相当。小規模なら自作でもOK。
* API例: `solve(VminusU, X, VplusU)`（右辺は行列）

7. **スクエアリング**

* 目的: $X \leftarrow X^{2^s}$ を繰り返し二乗で。
* API例: `for (i=0;i<s;i++) matmul(X,X,X);`

8. **ガード & 品質管理（簡易版）**

* 分母行列 $(V-U)$ のピボットが極端に小さい→ もう1段スケール/次数を上げる等のリカバリ
* 途中ノルムの過/アンダーフロー回避（スケール係数を持ちながら計算、など）
* 任意で残差 $\| e^{A_s} - X \|$ の簡易見積り（高コストなら省略）

---

# すぐ書ける骨格（C/BLAS風・最小）

```c
// 係数テーブル（例: m = 3,5,7,9,13 用）と閾値 theta[m] は別途定義
typedef struct { int m; const double *c; /* c0..c_{2m} */ double theta; } PadeTable;

// 1. ノルム
double mat_norm1(const double* A, int n);

// 2. スケーリングと次数
void choose_scale_and_order(double anorm, const PadeTable* tab, int* s, int* m) {
    // s: 最小の整数 s で anorm/2^s <= theta[m] を満たすように
    // m: anorm/2^s に応じた最小の m を選ぶ（表引き）
}

// 3. 本体
int expm_pade(const double* A, int n, double t, double* X /* out */) {
    // A_s = A * (t / 2^s)
    double anorm = mat_norm1(A, n) * fabs(t);
    int s, m;
    choose_scale_and_order(anorm, PADE_TABLE, &s, &m);

    // スケーリング
    double As[N*N];  // 実装では動的確保
    scale_matrix(A, n, t / (1<<s), As);

    // 必要冪の構築: A2 = As^2, A4 = A2^2, ...
    Powers P = compute_powers(As, n, m);   // 省略可: mごとに必要分だけ

    // U,V の構築（係数はテーブルから）
    double U[N*N], V[N*N], I[N*N];
    eye(I, n);
    form_UV(As, &P, m, U, V);  // V は I + 偶数次, U は 奇数次

    // (V - U) X = (V + U) を解く
    double VminusU[N*N], VplusU[N*N];
    mat_sub(V, U, VminusU, n);
    mat_add(V, U, VplusU, n);
    // 右辺は行列なので、列ごとに解く or ブロック版 dgesv を使う
    solve_linear_system(VminusU, X, VplusU, n); // X <- (V-U)^{-1}(V+U)

    // 二乗 s 回
    for (int i=0; i<s; ++i) matmul(X, X, X, n);

    return 0;
}
```

> ここで言う **Rmn の関数** は「係数テーブル＋U/V の合成＋(V−U)X=(V+U) の解法」のこと。
> exp 専用なら、**係数は固定**で毎回同じものを使います（あなたの理解どおり）。
> ただし “評価” は **毎回の $A t$** に対してやり直す必要があります。

---

## 追加であると嬉しいもの（後回し可）

* **LAPACK 連携**：`dgesv`/`dgetrf`/`dgetrs` を使えば安定＆実装負担が激減
* **ブロック化/再利用**：複数の $t$ を一括でやるときに冪を使い回す工夫
* **数値テスト**：既知ケース（対角/三角/正規行列）で相対誤差チェック
* **係数＆閾値の出典メモ**：README に「どの表を使っているか」を記載

---

### まとめ

> あなたの2点
> **1. ノルム関数**、**2. 固定の R\_{m,m}（＝係数表＋U/V 合成）**
> はコアですが、**“スケール選択→U/V構成→(V−U)X=(V+U)→スクエア”** の流れを乗せる必要があります。

必要なら、**係数表（m=3,5,7,9,13）** の実データと、**ノルム実装**＆\*\*LUソルバ（部分ピボット）\*\*を含む「そのままビルドできる最小コード」をまとめて渡します。
