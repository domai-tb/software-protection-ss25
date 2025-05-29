# Opaque Predicate 

Executing the script `solve_opaque_predicates.py` will output the following lines:

```zsh
Solving...
Solution found!
arg1 = 729
arg2 = 608
```

It uses the hardcoded conditions in `opaque_predicates.pseudo.c` and the Z3 solver to find correct inputs. 
Executing the given binary with the arguments `729` and `608` results in the `Correct!` output. 

```zsh
┌──(.env)─(domai㉿tbkali)-[~/Documents/software-protection-ss25/task-sheet-02]
└─$ ./material/opaque_predicates 729 608          
Correct!
```