from z3 import *
from random import choice, randint, getrandbits

# define z3 variables
x = BitVec("x", 64)
y = BitVec("y", 64)
z = BitVec("z", 64)

# define grammar components
variables = [x, y, z]
var_names = ["x", "y", "z"]
operators = ["+", "-", "*", "^", "&", "|"]
comparison = ["==", "!="]

def generate_random_expr(depth=3):
    """Generate a random expression with the given depth"""
    if depth == 0 or randint(0, 100) < 30:
        # Terminal: variable or constant
        if randint(0, 100) < 70:
            # Variable
            var = choice(variables)
            return var, var_names[variables.index(var)]
        else:
            # Constant
            const = getrandbits(64)
            return BitVecVal(const, 64), str(const)
    else:
        # Non-terminal: operation
        op = choice(operators)
        left_expr, left_str = generate_random_expr(depth - 1)
        right_expr, right_str = generate_random_expr(depth - 1)
        
        if op == "+":
            return left_expr + right_expr, f"{left_str} + {right_str}"
        elif op == "-":
            return left_expr - right_expr, f"{left_str} - {right_str}"
        elif op == "*":
            return left_expr * right_expr, f"{left_str} * {right_str}"
        elif op == "^":
            return left_expr ^ right_expr, f"{left_str} ^ {right_str}"
        elif op == "&":
            return left_expr & right_expr, f"{left_str} & {right_str}"
        elif op == "|":
            return left_expr | right_expr, f"{left_str} | {right_str}"

def generate_opaque_predicate():
    """Generate an opaque predicate"""
    # Generate left and right expressions
    left_expr, left_str = generate_random_expr()
    right_expr, right_str = generate_random_expr()
    
    # Choose comparison operator
    comp = choice(comparison)
    
    # Create the predicate
    if comp == "==":
        pred = left_expr == right_expr
        pred_str = f"{left_str} == {right_str}"
    else:  # comp == "!="
        pred = left_expr != right_expr
        pred_str = f"{left_str} != {right_str}"
    
    # Check if the predicate is opaque
    s = Solver()
    s.add(pred)
    
    if s.check() == unsat:
        # Always False predicate
        return pred_str, False
    
    s = Solver()
    s.add(Not(pred))
    
    if s.check() == unsat:
        # Always True predicate
        return pred_str, True
    
    # Not opaque (can be both True and False)
    return None, None

# set of unique opaque predicates
unique_opaque_predicates = set()

# search unique opaque predicates
while len(unique_opaque_predicates) < 10:
    result = generate_opaque_predicate()
    if result[0] is not None:
        unique_opaque_predicates.add(result)

# print generated opaque predicates
for index, (opaque_predicate, is_true) in enumerate(unique_opaque_predicates):
    print(f"opaque predicate {index}: {opaque_predicate} (Always {is_true})")