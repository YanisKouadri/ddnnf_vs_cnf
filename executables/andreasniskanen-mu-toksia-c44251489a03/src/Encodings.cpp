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

#include "Encodings.h"

using namespace std;

namespace Encodings {

void add_source_accepted_clauses(const DynamicAF & af, SAT_Solver * solver)
{
	if (af.static_mode) return;
	for (uint32_t i = 0; i < af.args; i++) {
		for (uint32_t j = 0; j < af.args; j++) {
			vector<int> clause = { af.att_exists_var(i,j), -af.source_accepted_var(i,j),  };
			solver->add_clause(clause);
		}
	}
	for (uint32_t i = 0; i < af.args; i++) {
		for (uint32_t j = 0; j < af.args; j++) {
			vector<int> clause = { af.accepted_var(i), -af.source_accepted_var(i,j), };
			solver->add_clause(clause);
		}
	}
	for (uint32_t i = 0; i < af.args; i++) {
		for (uint32_t j = 0; j < af.args; j++) {
			vector<int> clause = { -af.att_exists_var(i,j), -af.accepted_var(i), af.source_accepted_var(i,j) };
			solver->add_clause(clause);
		}
	}
}

void add_rejected_clauses(const DynamicAF & af, SAT_Solver * solver)
{
	for (uint32_t i = 0; i < af.args; i++) {
		if (af.static_mode && !af.arg_exists[i]) continue;
		vector<int> additional_clause = { -af.rejected_var(i), -af.accepted_var(i) };
		solver->add_clause(additional_clause);
	}
	if (af.static_mode) {
		for (uint32_t i = 0; i < af.args; i++) {
			if (!af.arg_exists[i]) continue;
			for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
				vector<int> clause = { af.rejected_var(i), -af.accepted_var(af.attackers[i][j]) };
				solver->add_clause(clause);
			}
		}
		for (uint32_t i = 0; i < af.args; i++) {
			if (!af.arg_exists[i]) continue;
			vector<int> clause(af.attackers[i].size() + 1);
			for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
				clause[j] = af.accepted_var(af.attackers[i][j]);
			}
			clause[af.attackers[i].size()] = -af.rejected_var(i);
			solver->add_clause(clause);
		}
	} else {
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause = { af.arg_exists_var(i), -af.rejected_var(i) };
			solver->add_clause(clause);
		}
		add_source_accepted_clauses(af, solver);
		for (uint32_t i = 0; i < af.args; i++) {
			for (uint32_t j = 0; j < af.args; j++) {
				vector<int> clause = { af.rejected_var(i), -af.source_accepted_var(j,i) };
				solver->add_clause(clause);
			}
		}
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause(af.args + 1);
			clause[0] = -af.rejected_var(i);
			for (uint32_t j = 0; j < af.args; j++) {
				clause[j+1] = af.source_accepted_var(j,i);
			}
			solver->add_clause(clause);
		}
	}
}

void add_source_rejected_clauses(const DynamicAF & af, SAT_Solver * solver)
{
	if (af.static_mode) return;
	for (uint32_t i = 0; i < af.args; i++) {
		for (uint32_t j = 0; j < af.args; j++) {
			vector<int> clause = { af.att_exists_var(i,j), af.source_rejected_var(i,j) };
			solver->add_clause(clause);
		}
	}
	for (uint32_t i = 0; i < af.args; i++) {
		for (uint32_t j = 0; j < af.args; j++) {
			vector<int> clause = { -af.rejected_var(i), af.source_rejected_var(i,j) };
			solver->add_clause(clause);
		}
	}
	for (uint32_t i = 0; i < af.args; i++) {
		for (uint32_t j = 0; j < af.args; j++) {
			vector<int> clause = { -af.att_exists_var(i,j), af.rejected_var(i), -af.source_rejected_var(i,j) };
			solver->add_clause(clause);
		}
	}
}

void add_range(const DynamicAF & af, SAT_Solver * solver)
{
	if (af.sem == STG) {
		if (af.static_mode) {
			for (uint32_t i = 0; i < af.args; i++) {
				if (!af.arg_exists[i]) continue;
				vector<int> clause = { af.range_var(i), -af.accepted_var(i) };
				solver->add_clause(clause);
			}
			for (uint32_t i = 0; i < af.args; i++) {
				if (!af.arg_exists[i]) continue;
				for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
					vector<int> clause = { af.range_var(i), -af.accepted_var(af.attackers[i][j]) };
					solver->add_clause(clause);
				}
			}
			for (uint32_t i = 0; i < af.args; i++) {
				if (!af.arg_exists[i]) continue;
				vector<int> clause(af.attackers[i].size() + 2);
				for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
					clause[j] = af.accepted_var(af.attackers[i][j]);
				}
				clause[af.attackers[i].size()] = af.accepted_var(i);
				clause[af.attackers[i].size()+1] = -af.range_var(i);
				solver->add_clause(clause);
			}
		} else {
			add_rejected_clauses(af, solver);
			for (uint32_t i = 0; i < af.args; i++) {
				vector<int> clause = { af.range_var(i), -af.accepted_var(i) };
				solver->add_clause(clause);
			}
			for (uint32_t i = 0; i < af.args; i++) {
				vector<int> clause = { af.range_var(i), -af.rejected_var(i) };
				solver->add_clause(clause);
			}
			for (uint32_t i = 0; i < af.args; i++) {
				vector<int> clause = { -af.range_var(i), af.accepted_var(i), af.rejected_var(i) };
				solver->add_clause(clause);
			}
		}
	} else {
		for (uint32_t i = 0; i < af.args; i++) {
			if (af.static_mode && !af.arg_exists[i]) continue;
			vector<int> clause = { af.range_var(i), -af.accepted_var(i) };
			solver->add_clause(clause);
		}
		for (uint32_t i = 0; i < af.args; i++) {
			if (af.static_mode && !af.arg_exists[i]) continue;
			vector<int> clause = { af.range_var(i), -af.rejected_var(i) };
			solver->add_clause(clause);
		}
		for (uint32_t i = 0; i < af.args; i++) {
			if (af.static_mode && !af.arg_exists[i]) continue;
			vector<int> clause = { -af.range_var(i), af.accepted_var(i), af.rejected_var(i) };
			solver->add_clause(clause);
		}
	}
}

void add_conflict_free(const DynamicAF & af, SAT_Solver * solver)
{
	if (af.static_mode) {
		for (uint32_t i = 0; i < af.args; i++) {
			if (!af.arg_exists[i]) continue;
			for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
				vector<int> clause;
				if ((int32_t)i != af.attackers[i][j]) {
					clause = { -af.accepted_var(i), -af.accepted_var(af.attackers[i][j]) };
				} else {
					clause = { -af.accepted_var(i) };
				}
				solver->add_clause(clause);
			}
		}
	} else {
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause = { af.arg_exists_var(i), -af.accepted_var(i) };
			solver->add_clause(clause);
		}
		for (uint32_t i = 0; i < af.args; i++) {
			for (uint32_t j = 0; j < af.args; j++) {
				vector<int> clause;
				if (i != j) {
					clause = { -af.att_exists_var(i,j), -af.accepted_var(i), -af.accepted_var(j) };
				} else {
					clause = { -af.att_exists_var(i,i), -af.accepted_var(i) };
				}
				solver->add_clause(clause);
			}
		}
	}
}

void add_admissible(const DynamicAF & af, SAT_Solver * solver)
{
	add_conflict_free(af, solver);
	add_rejected_clauses(af, solver);
	if (af.static_mode) {
		for (uint32_t i = 0; i < af.args; i++) {
			if (!af.arg_exists[i]) continue;
			if (af.self_attack[i]) continue;
			for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
				if (af.symmetric_attacks.count(make_pair(af.attackers[i][j], i))) continue;
				vector<int> clause = { -af.accepted_var(i), af.rejected_var(af.attackers[i][j]) };
				solver->add_clause(clause);
			}
		}
	} else {
		for (uint32_t i = 0; i < af.args; i++) {
			for (uint32_t j = 0; j < af.args; j++) {
				vector<int> clause = { -af.att_exists_var(j,i), -af.accepted_var(i), af.rejected_var(j) };
				solver->add_clause(clause);
			}
		}
	}
}

void add_complete(const DynamicAF & af, SAT_Solver * solver)
{
	add_admissible(af, solver);
	add_source_rejected_clauses(af, solver);
	if (af.static_mode) {
		for (uint32_t i = 0; i < af.args; i++) {
			if (!af.arg_exists[i]) continue;
			vector<int> clause(af.attackers[i].size()+1);
			clause[0] = af.accepted_var(i);
			for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
				clause[j+1] = -af.rejected_var(af.attackers[i][j]);
			}
			solver->add_clause(clause);
		}
	} else {
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause(af.args+2);
			clause[0] = -af.arg_exists_var(i);
			clause[1] = af.accepted_var(i);
			for (uint32_t j = 0; j < af.args; j++) {
				clause[j+2] = -af.source_rejected_var(j,i);
			}
			solver->add_clause(clause);
		}
	}
}

void add_stable(const DynamicAF & af, SAT_Solver * solver)
{
#if defined(AD_IN_ST)
	add_admissible(af, solver);
#elif defined(CO_IN_ST)
	add_complete(af, solver);
#else
	add_conflict_free(af, solver);
	add_rejected_clauses(af, solver);
#endif
	if (af.static_mode) {
		for (uint32_t i = 0; i < af.args; i++) {
			if (!af.arg_exists[i]) continue;
			vector<int> clause = { af.accepted_var(i), af.rejected_var(i) };
			solver->add_clause(clause);
		}
	} else {
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause = { -af.arg_exists_var(i), af.accepted_var(i), af.rejected_var(i) };
			solver->add_clause(clause);
		}
	}
}

int32_t add_target(DynamicAF & af, const vector<int32_t> & assumptions, SAT_Solver * solver)
{
	if (assumptions.size() == 0) return 0;
	if (assumptions.size() == 1) return assumptions[0];
	int32_t target_var = ++af.count;
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		vector<int> clause = { -target_var, assumptions[i] };
		solver->add_clause(clause);
	}
	vector<int> clause(assumptions.size()+1);
	clause[0] = target_var;
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		clause[i+1] = -assumptions[i];
	}
	solver->add_clause(clause);
	return target_var;
}

}