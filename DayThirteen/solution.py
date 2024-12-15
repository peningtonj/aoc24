from pulp import *
from itertools import islice
import re

def get_machines(filename):
    with open(filename) as f:
        while True:
            chunk = list(islice(f, 4))
            if not chunk:
                break
            yield chunk


def parse_input(filename):
    results = []
    for machine in get_machines(filename):
        pattern = re.compile(r".*X\+([0-9]*), Y\+([0-9]*)")
        aEffect = [int(x) for x in re.match(pattern, machine[0]).groups()]
        bEffect = [int(x) for x in re.match(pattern, machine[1]).groups()]
        pattern = re.compile(".*X=([0-9]*), Y=([0-9]*)")
        target = [int(x) for x in re.match(pattern, machine[2]).groups()]
        results.append((aEffect, bEffect, target))

    return results


def solve_problem(a, b, target):
    prob = LpProblem("Token_Minimisation", LpMinimize)
    aPresses = LpVariable("APresses", 0, None, LpInteger)
    bPresses = LpVariable("BPresses", 0, None, LpInteger)

    prob += (3 * aPresses) + bPresses

    prob += (aPresses * a[0]) + (bPresses * b[0]) == target[0] 
    prob += (aPresses * a[1]) + (bPresses * b[1]) == target[1] 

    prob.solve(PULP_CBC_CMD(msg=0))

    # print(prob.status)
    # print(a, b, target)
    # print(aPresses.value(), bPresses.value(), prob.objective.value())
    # print()

    return prob.objective.value() if prob.status != -1 else 0

def solve_big_problem(a, b, target):
    from sympy import symbols, Eq, solve

    # Define variables
    x, y = symbols('x y')

    # Create the equations
    eq1 = Eq(a[0]*x + b[0]*y, target[0])
    eq2 = Eq(a[1]*x + b[1]*y, target[1])

    # Solve the system of Diophantine equations
    solution = solve((eq1, eq2), (x, y))
    
    if (((solution[x] % 1) == 0) and ((solution[y] % 1) == 0)):
        return 3 * solution[x] + solution[y]
    
    else:
        return 0


def add_offset(problem):
    problem[2][0] += 10000000000000
    problem[2][1] += 10000000000000

    return problem



inputs = parse_input("input.txt")

print("Part 1: ", sum([solve_problem(*i) for i in inputs]))
inputs = [add_offset(p) for p in inputs]

results = [solve_big_problem(*i) for i in inputs]

print("Part 2:", sum(results))


