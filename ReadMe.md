# Submodular Knapsack-Constrained Maximization

This repository contains the code presented in the paper **An Exact Solver for Submodular Knapsack Problems** by Sabine Münch and Stephen Raach.

## Submodular Knapsack-Constrained Maximization

Let $f:2^I\rightarrow \mathbb{R}_{\geq 0}$ be a submodular ($f(X\cup\{i\})-f(X) \geq f(Y\cup \{i\})-f(Y)$ for $X\subseteq Y\subseteq I$, $i \in I\setminus Y$), monotone increasing ($f(X)\leq f(Y)$ for any $X\subseteq Y\subseteq I$), and normalized ($f(\emptyset)=0$) function.

Then, the Submodular Knapsack-Constrained Maximization problem is given by: $\max \{f(X)\colon w(X) \leq B,\, X\subseteq I\}$

Currently, three objective functions can be optimized.

#### Weighted Coverage
Let $I=\{1,\dots,n\}$ and $E=\{1,\dots,m\}$ with $n,m\in \mathbb{N}$. Let $\{E_i\colon E_i\subseteq E, i\in I\}$ be a set of subsets of $E$. Each element $e\in E$ is associated with a value $v_e\geq 0$ and each $i\in I$ with a weight $w_{i}>0$. The weighted coverage problem is given by:

$$
\max_{X\subseteq I, w(X)\leq B} f(X)
$$

with $f\colon 2^{I}\rightarrow \mathbb{R}_{\geq 0}, X\mapsto \sum_{e\in\bigcup_{i\in X}E_i}v_e$.
    


#### Facility Location
Let $I=\{1,\dots,n\}$, $n\in\mathbb{N}$ be a set of locations and $M=\{1,\dots,m\}$, $m\in \mathbb{N}$ be a set of customers. Each $i\in I$ is associated with some costs $w_i\geq 0$ and $v_{i,j}$ is the benefit for customer $j\in M$ if served by the facility in location $i$. Let $B\geq 0$ be a given budget. Then, the facility location problem is defined by 

$$
\max_{S\subseteq I, w(S)\leq B} \sum_{j\in M} \max_{i\in S}v_{i,j}.
$$



#### Bipartite Influence
Let $I=\{1,\dots,n\}$ with $n \in \mathbb{N}$ be a set of sources and $M=\{1,\dots,m\}$ with $m\in\mathbb{N}$ be a set of targets. Let $G=(I\cup M, A)$ be a bipartite directed graph, where $A\subseteq I\times M$ is a set of directed edges.
The probability that a target $j\in M$ is activated by a set $S\subseteq I$ is $1-\prod_{i\in S\colon (ij)\in A} (1-p_i)$, where $0\leq p_i\leq 1$ is the activation probability of source $i$.
Every source $i\in I$ is associated with activation costs $w_i\geq 0$, and $B\geq 0$ is a given budget.
The objective is to maximize the expected number of targets that are activated by a set $S\subseteq I$ with $w(S)\leq B$: 

$$
\max_{S\subseteq I, w(S)\leq B} \sum_{j\in M} \left(1-\prod_{i\in S\colon (ij)\in A}(1-p_i)\right).
$$


## Installation

### Build
To build the binary, use `make`.

### Run
To run use `./main B x y`, 

where $B$ is the knapsack capacity. $B$ should be an integer,

$x\in \{0,1,2\}$ specifies the objective function ($0$ -> weighted coverage, $1$ -> facility location, $2$ -> bipartite inference), and

$y\in \{0,1,2,3,4,5,6,7\}$ specifies the solver ($0$ -> basic branch-and-bound (Algorithm 1), $1$ -> Candidate Reduction (CR), $2$ -> Lazy Evaluations with the average decision rule (LE<sup>a</sup>), $3$ -> Early Pruning , $4$ -> Combination of Lazy Evaluations (with the average decision rule) and Candidate Reduction (LE<sup>a</sup>+CR), $5$ -> Combination of Early Pruning and Candidate Reduction (EP+CR), $6$ -> Combination of Lazy Evaluations (with the average decision rule) and Early Pruning (LE<sup>a</sup>+EP), $7$ -> Combination of Lazy Evaluations (with the average decision rule), Early Pruning and Candidate Reduction (LE<sup>a</sup>+EP+CR), $8$ -> Lazy Evaluations with the greedy decision rule (LE<sup>g</sup>) ).

### Time Limit

The time limit of all test is set to one hour.

### Data

#### Weighted Coverage
Items: A file COV_items.json contains a list of $n$ items, each represented by the elements it covers from the ground set. COV_items.json should be formatted like this:

```
[[e11, e12,..., e1k_1],[e21,e22,..., e2k_2], ... , [en1, en2,..., enk_n]]
```

- Each $e_{xy}$ is an element from the ground set covered by the item $i_x$.
- Each $e_{xk_x}$ is the last element covered by item $i_x$. Hence, each item $i_x$ covers $k_x$ elements.
- Items do not need to cover the same number of elements from the ground set

Weights: A file COV_Itemsweights.json contains a list of all item weights. COV_Itemsweights.json should be formatted as follows:

```
[w(i1), w(i2), ... , w(in)]
```

- Each $w(i_x)$ should be a non-negative integer, which denotes the weight of item $i_x$.

Values:  A file COV_Groundsetvalues.json contains a list of all values of elements in the ground set. COV_Groundsetvalues.json should be formatted as follows:

```
[v(e1), v(e2), ... , v(em)]
```

- Each $v(e_x)$ should be a non-negative float, which denotes the value of the element $e_x$ from the ground set.

#### Facility Location
Benefits: A file LOC_benefits.json contains a $n\times m$ matrix of benefits. LOC_benefits.json should be formatted as follows:

```
[[v11, v12,..., v1n],[v21,v22,..., v2n], ... , [vm1, vm2,..., vmn]]
```

- Each $v_{i,j}$ with $i\in I$ and $j\in M$ should be a non-negative float, which denotes the benefit of customer $j$ to be served by the facility in location $i$.

Weights: A file LOC_weights.json contains a list of all location costs. LOC_weights.json should be formatted as follows:

```
[w(i1), w(i2), ... , w(in)]
```

- Each $w(i_x)$ should be a non-negative integer, which denotes the costs of location $i_x$.

#### Bipartite Influence

Connections: A file INF_connections.json contains a matrix representing the directed graph $G$. INF_connections.json should be formatted as follows:

```
[[c11, c12, ... ,c1m],[c21,c22, ... ,c2m], ... ,[cn1, cn2, ... ,cnm]]
```

- Each $c_{i,j}$ with $i\in I$ and $j\in M$ should be a float, that is $1$ if $(i,j)\in A$ and $0$ if $(i,j)\not\in A$.



Probability: A file INF_probability.json contains a list of all activation probabilities. INF_probability.json should be formatted as follows:

```
[p(i1), p(i2), ... , p(in)]
```

- Each $p(i_x)$ should be a non-negative float, which denotes the activation probability of source $i_x$.

Weights: A file INF_weights.json contains a list of all activation costs. INF_weights.json should be formatted as follows:

```
[w(i1), w(i2), ... , w(in)]
```

- Each $w(i_x)$ should be a non-negative integer, which denotes the activation costs of source $i_x$.