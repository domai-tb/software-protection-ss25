from z3 import *
from random import choice, getrandbits

# define z3 variables
x = BitVec("x", 64)
y = BitVec("y", 64)
z = BitVec("z", 64)

# define grammar components
variables = ["x", "y", "z"]
operators = ["+", "-", "*", "^", "&", "|"]
comparison = ["==", "!="]


# set of unique opaque predicates
unique_opaque_predicates = set()

# search unique opaque predicates
while len(unique_opaque_predicates) != 10:
	pass


# print generated opaque predicates
for index, opaque_predicate in enumerate(unique_opaque_predicates):
	print(f"opaque predicate {index}: {opaque_predicate}")