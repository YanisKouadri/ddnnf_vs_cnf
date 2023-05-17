/*!
 * Copyright (c) <2023> <Andreas Niskanen, University of Helsinki>
 * 
 * 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * 
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "GlucoseSolver.h"

using namespace std;
using namespace Glucose;

GlucoseSolver::GlucoseSolver(int32_t n_vars, int32_t n_args)
{
	solver = new Solver();
#if defined(INCREMENTAL)
	solver->setIncrementalMode();
	solver->initNbInitialVars(n_args);
#endif
	decision_vars = n_args;
}

void GlucoseSolver::add_clause(const vector<int32_t> & clause)
{
	vec<Lit> lits(clause.size());
	for (uint32_t i = 0; i < clause.size(); i++) {
		int32_t var = abs(clause[i])-1;
		while (var >= solver->nVars())
			solver->newVar();
		lits[i] = (clause[i] > 0) ? mkLit(var) : ~mkLit(var);
	}
	solver->addClause_(lits);
}

void GlucoseSolver::assume(int32_t lit)
{
	int32_t var = abs(lit)-1;
	while (var >= solver->nVars())
		solver->newVar();
	assumptions.push((lit > 0) ? mkLit(var) : ~mkLit(var));
}

bool GlucoseSolver::solve()
{
	bool sat = solver->solve(assumptions);
	if (sat) {
		assignment.clear();
		for (int32_t i = 0; i < decision_vars; i++) {
			assignment.push_back(solver->modelValue(i) == l_True ? 1 : 0);
		}
	}
	assumptions.clear();
	return sat;
}

bool GlucoseSolver::propagate(vector<int32_t> & out_lits)
{
	for (int32_t i = 0; i < assumptions.size(); i++) {
		solver->newDecisionLevel();
		solver->uncheckedEnqueue(assumptions[i]);
	}
	assumptions.clear();
	CRef conflict = solver->propagate();
	for (int32_t i = 0; i < decision_vars; i++) {
		if (solver->value(i) == l_True)
			out_lits.push_back(i+1);
		else if (solver->value(i) == l_False)
			out_lits.push_back(-(i+1));
	}
	solver->cancelUntil(0);
	return conflict == CRef_Undef;
}

bool GlucoseSolver::get_value(int32_t lit)
{
	int32_t var = abs(lit)-1;
	lbool val = solver->modelValue(var);
	return (lit > 0) ? val == l_True : val == l_False;
}