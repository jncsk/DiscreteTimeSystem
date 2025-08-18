# Matrix Exponential Discretization Method

<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [Matrix Exponential Discretization Method](#matrix-exponential-discretization-method)
  - [1. 問題設定](#1-問題設定)
  - [2. Van Loan のアイデア](#2-van-loan-のアイデア)
  - [3. 証明](#3-証明)
    - [(1) 行列指数の級数展開](#1-行列指数の級数展開)
    - [(2) $M^k$ の形](#2-mk-の形)
    - [(3) 級数に代入](#3-級数に代入)
    - [(4) ブロックごとの和](#4-ブロックごとの和)
    - [(5) 結果](#5-結果)
  - [4. 出典](#4-出典)

<!-- /code_chunk_output -->


---

## 1. 問題設定
連続時間の状態方程式：

$$
\begin{equation}
    \dot{x}(t) = A x(t) + B u(t)
\end{equation}
$$

をサンプリング周期 $h$ で ZOH 入力とすると、離散時間モデルは：

$$
\begin{equation}
    x_{k+1} = A_d x_k + B_d u_k
\end{equation}
$$

ここで

$$
\begin{equation}
    A_d = e^{Ah}, \qquad B_d = \int_0^h e^{A\tau} B \, d\tau
\end{equation}
$$

が必要となる。

---

## 2. Van Loan のアイデア
次の拡大ブロック行列を定義する：

$$
\begin{equation}
    M = \begin{bmatrix}
    A & B \\
    0 & 0
\end{bmatrix}
\end{equation}
$$

この指数関数を展開すると：


$$
\begin{equation}
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
\end{equation}
$$

よって、**ブロック行列の指数を一度計算すれば $A_d, B_d$ を同時に得られる**。

---

## 3. 証明

### (1) 行列指数の級数展開
$$
\begin{equation}
    e^{Mh} = \sum_{k=0}^{\infty} \frac{h^k}{k!}M^k
\end{equation}
$$

### (2) $M^k$ の形
帰納法で：

$$
\begin{equation}
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
\end{equation}
$$

### (3) 級数に代入
$$
\begin{equation}
    (Mh)^k = h^k M^k
    = \begin{bmatrix}
    h^k A^k & h^k A^{k-1}B \\
    0 & 0
    \end{bmatrix}, \quad (k \ge 1)
\end{equation}
$$

したがって：

$$
\begin{equation}
\begin{aligned}
e^{Mh} 
    &= \frac{(Mh)^0}{0!} + \sum_{k=1}^{\infty} \frac{(Mh)^k}{k!} \\
    &= \frac{(Mh)^0}{0!} + \sum_{k=0}^{\infty} \frac{h^k}{k!}M^k \\
    &= \frac{(Mh)^0}{0!} + \sum_{k=0}^{\infty} \frac{h^k}{k!}
        \begin{bmatrix}
        A^k & A^{k-1}B \\
        0 & 0
        \end{bmatrix}
\end{aligned}
\end{equation}
$$

### (4) ブロックごとの和
行列の和はブロックごとに取れるため：

- 左上ブロック：
$$
\begin{equation}
    I + \sum_{k=1}^\infty \frac{h^k}{k!} A^k = e^{Ah}
\end{equation}
$$

- 右上ブロック：
$$
\begin{equation}
    \sum_{k=1}^\infty \frac{h^k}{k!} A^{k-1}B
\end{equation}
$$
ここで添字変換 $m=k-1$ を行うと：

$$
\begin{equation}
    = \sum_{m=0}^\infty \frac{h^{m+1}}{(m+1)!} A^m B
\end{equation}
$$

さらに：

$$
\begin{equation}
    \sum_{m=0}^\infty \frac{h^{m+1}}{(m+1)!} A^m
    = \int_0^h \left( \sum_{m=0}^\infty \frac{(A\tau)^m}{m!} \right) d\tau
    = \int_0^h e^{A\tau}\, d\tau
\end{equation}
$$

よって：

$$
\begin{equation}
    B_d = \left(\int_0^h e^{A\tau}\, d\tau\right) B
\end{equation}
$$

- 左下ブロック：$0$  

- 右下ブロック：$I$  

### (5) 結果
以上より、
$$
\begin{equation}
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
\end{equation}
$$

となる。したがって、拡大行列$M$の行列指数関数の計算により
- 左上ブロック： $A_d = e^{Ah}$
- 右上ブロック： $B_d = \int_0^h e^{A\tau}B , d\tau$
が一度で同時に得られる。

---

## 4. 出典
C. Van Loan, “Computing Integrals Involving the Matrix Exponential,” *IEEE Transactions on Automatic Control*, 23(3), 1978.
