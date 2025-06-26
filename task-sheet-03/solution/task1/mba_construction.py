from random import randint
from miasm.expression.expression import ExprId, ExprInt
from miasm.ir.translators.z3_ir import TranslatorZ3
from z3 import Solver, unsat


def generate_invertible_function() -> tuple[ExprInt, ExprInt, ExprInt, ExprInt]:
    """
    Generates parameters (m, n, a, b) for an invertible linear function f(x) = m*x + a
    and its inverse f^-1(x) = n*x + b, where operations are performed modulo 256.

    The function ensures that m and n are multiplicative inverses modulo 256.
    Also finds 'a' and 'b' such that f(f^-1(x)) = x mod 256.
    """
    found_multiplier = False
    m: int = 0
    n: int = 0
    while not found_multiplier:
        multiplier = randint(2, 255)
        tmp1 = (multiplier * 256) + 1
        m_val = 255
        while m_val > 2:
            if tmp1 % m_val == 0 and (tmp1 // m_val) < 256:
                break
            m_val -= 1
        n_val = tmp1 // m_val
        if m_val > 2 and m_val != n_val:
            m = m_val
            n = n_val
            found_multiplier = True

    b_val = randint(2, 255)
    a_val = 1
    while ((b_val * m) + a_val) % 256 != 0:
        a_val += 1

    x_ = ExprId("x", 8)
    m_ = ExprInt(m, 8)
    n_ = ExprInt(n, 8)
    a_ = ExprInt(a_val, 8)
    b_ = ExprInt(b_val, 8)

    return m_, n_, a_, b_


def prove_mba_equivalence(
    m: ExprInt,
    n: ExprInt,
    a: ExprInt,
    b: ExprInt,
    translator: TranslatorZ3,
    solver: Solver,
) -> bool:
    """
    Proves that a specific MBA (Mixed Boolean-Arithmetic) expression
    constructed from m, n, a, b is equivalent to x + y.

    Args:
        m: Miasm expression for m.
        n: Miasm expression for n.
        a: Miasm expression for a.
        b: Miasm expression for b.
        translator: Z3 translator instance.
        solver: Z3 solver instance.

    Returns:
        True if the MBA is equivalent to x + y, False otherwise.
    """
    x = ExprId("x", 8)
    y = ExprId("y", 8)
    two = ExprInt(2, 8)

    # Original expression: m*(n*((x^y) + two*(x&y)) + b) + a
    # This simplifies to m*(n*(x+y) + b) + a due to the identity (x^y) + 2*(x&y) = x+y
    mba_expr = m * (n * ((x ^ y) + two * (x & y)) + b) + a
    target_expr = x + y

    solver.reset()
    solver.add(translator.from_expr(mba_expr) != translator.from_expr(target_expr))

    return solver.check() == unsat


if __name__ == "__main__":
    translator = TranslatorZ3()
    solver = Solver()

    generated_m_values: list[int] = []
    num_mb_expressions_to_generate = 10
    current_count = 0

    print(
        f"\n--- Generating {num_mb_expressions_to_generate} MBAs equivalent to x + y ---\n"
    )

    while current_count < num_mb_expressions_to_generate:
        m_expr, n_expr, a_expr, b_expr = generate_invertible_function()

        # Extract integer value from ExprInt for uniqueness check
        m_val = m_expr.arg

        if m_val in generated_m_values:
            continue

        generated_m_values.append(m_val)

        if prove_mba_equivalence(m_expr, n_expr, a_expr, b_expr, translator, solver):
            current_count += 1
            # Reconstruct the MBA expression for printing, similar to the original script
            x_print = ExprId("x", 8)
            y_print = ExprId("y", 8)
            two_print = ExprInt(2, 8)
            mba_display_expr = (
                m_expr
                * (
                    n_expr * ((x_print ^ y_print) + two_print * (x_print & y_print))
                    + b_expr
                )
                + a_expr
            )

            print(f"[{current_count}/{num_mb_expressions_to_generate}] Generated MBA:")
            print(f" \t{mba_display_expr}\n")
        else:
            print(
                f"ERROR: MBA with m={m_val} was generated but not equivalent to x + y. Skipping.\n"
            )
