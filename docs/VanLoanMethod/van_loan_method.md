# Van Loan 法（Block Matrix Exponential Method）の理論展開

## 1. 問題設定
連続時間の状態方程式：

$$
\dot{x}(t) = A x(t) + B u(t)
$$

をサンプリング周期 $h$ で ZOH 入力とすると、離散時間モデルは：

$$
x_{k+1} = A_d x_k + B_d u_k
$$

ここで

$$
A_d = e^{Ah}, \qquad B_d = \int_0^h e^{A\tau} B \, d\tau
$$

が必要となる。特に $B_d$ の計算が課題となる。

---

## 2. Van Loan のアイデア
次の拡大ブロック行列を構成する：

$$
M = \begin{bmatrix}
A & B \\
0 & 0
\end{bmatrix}
$$

この指数関数を展開すると：

$$
e^{Mh} =
\begin{bmatrix}
e^{Ah} & \int_0^h e^{A\tau}B \, d\tau \\
0 & I
\end{bmatrix}
=
\begin{bmatrix}
A_d & B_d \\
0 & I
\end{bmatrix}
$$

よって、**ブロック行列の指数を一度計算すれば $A_d, B_d$ を同時に得られる**。

---

## 3. べき級数からの直接計算

### (1) 行列指数の級数展開
$$
e^{Mh} = \sum_{k=0}^{\infty} \frac{(Mh)^k}{k!}
$$

ここで

$$
M = \begin{bmatrix}
A & B \\
0 & 0
\end{bmatrix}
$$

### (2) $M^k$ の形
帰納法で：

$$
M^k =
\begin{bmatrix}
A^k & A^{k-1}B \\
0 & 0
\end{bmatrix}, \quad (k \ge 1),
\qquad
M^0 =
\begin{bmatrix}
I & 0 \\
0 & I
\end{bmatrix}
$$

### (3) 級数に代入
$$
(Mh)^k = h^k M^k
= \begin{bmatrix}
h^k A^k & h^k A^{k-1}B \\
0 & 0
\end{bmatrix}, \quad (k \ge 1)
$$

したがって：

$$
e^{Mh} = \frac{(Mh)^0}{0!} + \sum_{k=1}^\infty \frac{(Mh)^k}{k!}
$$

### (4) ブロックごとの和
行列の和はブロックごとに取れるため：

- 左上ブロック：
$$
I + \sum_{k=1}^\infty \frac{h^k}{k!} A^k = e^{Ah}
$$

- 右上ブロック：
$$
\sum_{k=1}^\infty \frac{h^k}{k!} A^{k-1}B
$$

ここで添字変換 $m=k-1$ を行うと：

$$
= \sum_{m=0}^\infty \frac{h^{m+1}}{(m+1)!} A^m B
$$

さらに：

$$
\sum_{m=0}^\infty \frac{h^{m+1}}{(m+1)!} A^m
= \int_0^h \left( \sum_{m=0}^\infty \frac{(A\tau)^m}{m!} \right) d\tau
= \int_0^h e^{A\tau}\, d\tau
$$

よって：

$$
B_d = \left(\int_0^h e^{A\tau}\, d\tau\right) B
$$

- 左下ブロック：$0$  

- 右下ブロック：$I$  

### (5) 結果

$$
e^{Mh} =
\begin{bmatrix}
e^{Ah} & \int_0^h e^{A\tau}B \, d\tau \\
0 & I
\end{bmatrix}
=
\begin{bmatrix}
A_d & B_d \\
0 & I
\end{bmatrix}
$$

---

## 4. ポイントまとめ
- 各 $(Mh)^k$ がブロック行列の形をしているため、和もブロックごとに展開できる  
- 左上ブロックはそのまま行列指数 $e^{Ah}$  
- 右上ブロックは級数を積分に書き換えることで $\int_0^h e^{A\tau}B\,d\tau$ になる  
- よって Van Loan 法の主張が成立  

---

## 5. 名称について
- 日本語文献： **Van Loan 法** と呼ばれることが多い  
- 英語圏： **block matrix exponential method**, **matrix exponential discretization method**  
- 出典： C. Van Loan, “Computing Integrals Involving the Matrix Exponential,” *IEEE Transactions on Automatic Control*, 23(3), 1978.
