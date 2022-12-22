"""
A simple interpreter.

sleep 100  -- sleep for 100 milliseconds
for i to 10 do ... end   -- repeat 10 times
random  -- print random word
abc  -- print "abc"
i  -- print the value of i (when i is a loop variable)
"""

import random
import string
import sys
import time


def compile(tokens):
    i = 0
    n = len(tokens)

    def statement(vars):
        nonlocal i, n
        commands = []
        while i < n:
            if tokens[i] == "end":
                break
            if tokens[i] == "for":
                i += 1
                var = tokens[i]
                assert var not in vars, "variable shadowing"
                i += 1
                to = tokens[i]
                assert to == "to", "expected 'to'"
                i += 1
                stop = int(tokens[i])
                i += 1
                do = tokens[i]
                assert do == "do", "expected 'do'"
                i += 1
                stmt = statement(vars + (var,))
                end = tokens[i]
                assert end == "end", "expected 'end'"
                i += 1
                commands.append(("for", var, stop, stmt))
            elif tokens[i] == "sleep":
                i += 1
                milliseconds = float(tokens[i])
                i += 1
                commands.append(("sleep", milliseconds / 1000))
            elif tokens[i] == "random":
                i += 1
                commands.append(("print_random",))
            else:
                x = tokens[i]
                i += 1
                if x in vars:
                    commands.append(("print_var", x))
                else:
                    commands.append(("print_const", x))
        return tuple(commands)

    return statement(())


vowels = "aeiouy"
consonants = "".join(set(string.ascii_lowercase) - set(vowels))


def execute(statements, vars):
    for (stmt, *args) in statements:
        if stmt == "for":
            var, iters, stmts = args
            for i in range(1, iters + 1):
                execute(stmts, {**vars, var: i})
        elif stmt == "print_var":
            var, = args
            print(vars[var], flush=True)
        elif stmt == "print_random":
            print("".join(
                (random.choice(consonants) + random.choice(vowels) for _ in
                 range(random.randint(6, 10)))), flush=True)
        elif stmt == "print_const":
            s, = args
            print(s, flush=True)
        elif stmt == "sleep":
            t, = args
            time.sleep(t)
        else:
            assert False


commands = (" ".join(sys.argv[1:])).lower().split()
if not commands:
    commands = "for i to 10000 do random sleep 10 end".split()
statements = compile(commands)
print(statements)
execute(statements, {})
