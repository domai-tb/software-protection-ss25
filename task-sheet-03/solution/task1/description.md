# Task 1: MBA Construction

The algorithm implemented in the provided Python code can be broken down into two main parts:

## Part 1: Generating Invertible Functions

This part focuses on crafting the invertible function `f(x) = m * x + a` and `f'(x) = n * x + b` on byte level / modulo 256.

The goal is to find four specific numbers: `m`, `n`, `a`, and `b`, such that:

1.  `f(x) = (m * x + a) % 256`
2.  `f'(x) = (n * x + b) % 256`
3.  When you apply `f` and then `f'` (or vice versa), you get back the original number. This means `f(f'(x)) = x % 256`.

The process of finding these numbers is as follows:

- It first randomly picks a "multiplier" between 2 and 255.
- It then tries to find two numbers, `m` and `n`, such that their product, `m * n`, is equivalent to `1 % 256`. This makes `m` and `n` multiplicative inverses modulo 256. The code achieves this by looking for factors of `(multiplier * 256) + 1`.
- Once `m` and `n` are found, it then searches for the smallest positive integer `a` that satisfies the equation `(m * b + a) % 256 = 0`, where `b` is a randomly chosen number between 2 and 255. This ensures the inverse property holds.

## Part 2: Proving Equivalence using Z3

This part takes the `m, n, a, b` values generated in the first part and uses them to construct a MBA:
`m * (n * ((x ^ y) + 2 * (x & y)) + b) + a`

The core idea here is that the sub-expression `(x ^ y) + 2 * (x & y)` is a known identity of `x + y`.

Therefore, the entire MBA expression simplifies to:
`m * (n * (x + y) + b) + a`

Since `m * n = 1 % 256` and `(m * b + a) % 256 = 0`, the expression further simplifies to:
`(m * n * (x+y) + m * b + a) % 256 = (1 * (x+y) + 0) % 256 = (x+y) % 256`.

The code then uses the Z3 theorem prover.
