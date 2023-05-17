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

#include "mu-toksia.h"
#include "Encodings.h"

using namespace std;

AFSolver::AFSolver()
{
	sat_solver = nullptr;
	solver_encoding = UNKNOWN_SEM;
	current_state = INPUT;
	buffer_size = INITIAL_BUFFER_SIZE;
}

void AFSolver::set_semantics(sigma s)
{
	dynamic_af.sem = s;
}

void AFSolver::add_argument(int32_t arg)
{
	if (dynamic_af.args_remaining == 0) {
		dynamic_af.vars_initialized = false;
	}
	bool success = dynamic_af.add_argument(arg);
	if (!success) current_state = ERROR;
}

void AFSolver::del_argument(int32_t arg)
{
	bool success = dynamic_af.del_argument(arg);
	if (!success) current_state = ERROR;
}

void AFSolver::add_attack(int32_t source, int32_t target)
{
	bool success = dynamic_af.add_attack(source, target);
	if (!success) current_state = ERROR;
}

void AFSolver::del_attack(int32_t source, int32_t target)
{
	bool success = dynamic_af.del_attack(source, target);
	if (!success) current_state = ERROR;
}

void AFSolver::assume_in(int32_t arg)
{
	if (dynamic_af.arg_to_int.count(abs(arg)) == 0) {
		current_state = ERROR;
		return;
	}
	int32_t arg_index = dynamic_af.arg_to_int.at(abs(arg));
	int32_t var = dynamic_af.accepted_var(arg_index);
	status_assumptions.push_back(arg > 0 ? var : -var);
}

void AFSolver::assume_out(int32_t arg)
{
	if (dynamic_af.arg_to_int.count(abs(arg)) == 0) {
		current_state = ERROR;
		return;
	}
	int32_t arg_index = dynamic_af.arg_to_int.at(abs(arg));
	int32_t var = dynamic_af.rejected_var(arg_index);
	status_assumptions.push_back(arg > 0 ? var : -var);
}

void AFSolver::assume_current_structure(SAT_Solver * solver)
{
	if (dynamic_af.static_mode) return;
	for (uint32_t i = 0; i < dynamic_af.args; i++) {
		if (dynamic_af.arg_exists[i])
			solver->assume(dynamic_af.arg_exists_var(i));
		else
			solver->assume(-dynamic_af.arg_exists_var(i));
	}
	for (uint32_t i = 0; i < dynamic_af.args; i++) {
		for (uint32_t j = 0; j < dynamic_af.args; j++) {
			if (dynamic_af.attacks.count(make_pair(i,j)))
				solver->assume(dynamic_af.att_exists_var(i,j));
			else
				solver->assume(-dynamic_af.att_exists_var(i,j));
		}
	}
}

void AFSolver::assume_grounded(SAT_Solver * solver)
{
	SAT_Solver * propagator = new SAT_Solver(dynamic_af.count, 2*dynamic_af.args);
	Encodings::add_complete(dynamic_af, propagator);
	assume_current_structure(propagator);
	vector<int32_t> grounded;
	propagator->propagate(grounded);
	for (uint32_t i = 0; i < grounded.size(); i++)
		solver->assume(grounded[i]);
	if (propagator) delete propagator;
}

void AFSolver::assume_range(SAT_Solver * solver)
{
	for (uint32_t i = 0; i < dynamic_af.args; i++) {
		if (dynamic_af.arg_exists[i])
			solver->assume(dynamic_af.range_var(i));
	}
}

void AFSolver::set_objective_vars()
{
	objective_vars.clear();
	objective_vars.reserve(dynamic_af.args);
	if (dynamic_af.sem == PR) {
		for (uint32_t i = 0; i < dynamic_af.args; i++) {
			if (dynamic_af.arg_exists[i])
				objective_vars.push_back(dynamic_af.accepted_var(i));
		}
	} else if (dynamic_af.sem == SST || dynamic_af.sem == STG) {
		for (uint32_t i = 0; i < dynamic_af.args; i++) {
			if (dynamic_af.arg_exists[i])
				objective_vars.push_back(dynamic_af.range_var(i));
		}
	}
}

bool AFSolver::check_extensions(int32_t target, int32_t select)
{
	for (auto it = extensions.rbegin(); it != extensions.rend(); ++it) {
		assume_current_structure(sat_solver);
		if (target) sat_solver->assume(target);
		for (uint32_t i = 0; i < dynamic_af.args; i++) {
			if ((*it)[i]) sat_solver->assume(dynamic_af.accepted_var(i));
			else sat_solver->assume(-dynamic_af.accepted_var(i));
		}
		if (sat_solver->solve()) {
			if (dynamic_af.sem == CO || dynamic_af.sem == ST) {
				return true;
			} else if (dynamic_af.sem == PR || dynamic_af.sem == SST || dynamic_af.sem == STG) {
				assume_current_structure(sat_solver);
				if (select) sat_solver->assume(-select);
				vector<int> complement_clause;
				if (select) complement_clause.push_back(select);
				for (uint32_t i = 0; i < objective_vars.size(); i++) {
					if (sat_solver->get_value(objective_vars[i])) {
						sat_solver->assume(objective_vars[i]);
					} else {
						complement_clause.push_back(objective_vars[i]);
					}
				}
				sat_solver->add_clause(complement_clause);
				bool superset_exists = sat_solver->solve();
				if (!superset_exists) return true;
			}
		}
	}
	return false;
}

bool AFSolver::cegar(int32_t target, int32_t select)
{
	while (true) {
		assume_current_structure(sat_solver);
		if (select) sat_solver->assume(-select);
		if (target) sat_solver->assume(target);
		bool sat = sat_solver->solve();
		if (!sat) break;

		while (true) {
			assume_current_structure(sat_solver);
			if (select) sat_solver->assume(-select);
			if (target) sat_solver->assume(target);
			vector<int> complement_clause;
			if (select) complement_clause.push_back(select);
			for (uint32_t i = 0; i < objective_vars.size(); i++) {
				if (sat_solver->get_value(objective_vars[i])) {
					sat_solver->assume(objective_vars[i]);
				} else {
					complement_clause.push_back(objective_vars[i]);
				}
			}
			sat_solver->add_clause(complement_clause);
			bool superset_exists = sat_solver->solve();
			if (!superset_exists) {
				if (target) break;
				extensions.push_back(sat_solver->assignment);
				return true;
			}
		}

		assume_current_structure(sat_solver);
		if (select) sat_solver->assume(-select);
		for (uint32_t i = 0; i < objective_vars.size(); i++) {
			if (sat_solver->get_value(objective_vars[i])) {
				sat_solver->assume(objective_vars[i]);
			}
		}
		if (!sat_solver->solve()) {
			extensions.push_back(sat_solver->assignment);
			return true;
		}
	}
	return false;
}

bool AFSolver::ideal(int32_t target, int32_t select) {

	vector<int> accepted_clause;
	vector<int> rejected_clause;
	vector<uint8_t> union_of_accepted(dynamic_af.args, 0);
	vector<uint8_t> union_of_rejected(dynamic_af.args, 0);	
	bool solved = false;

	while (true) {
		assume_current_structure(sat_solver);
		if (!solved && target) sat_solver->assume(target);
		if (solved && select) sat_solver->assume(-select);
		bool sat = sat_solver->solve();
		if (!sat) {
			if (!solved && target) return false;
			break;
		}
		solved = true;
		accepted_clause.clear();
		rejected_clause.clear();
		if (select) accepted_clause.push_back(select);
		if (select) rejected_clause.push_back(select);
		for (uint32_t i = 0; i < dynamic_af.args; i++) {
			if (!dynamic_af.arg_exists[i]) continue;
			if (sat_solver->get_value(dynamic_af.accepted_var(i))) {
				union_of_accepted[i] = 1;
			} else if (!union_of_accepted[i]) {
				accepted_clause.push_back(dynamic_af.accepted_var(i));
			}
			if (sat_solver->get_value(dynamic_af.rejected_var(i))) {
				union_of_rejected[i] = 1;
			} else if (!union_of_rejected[i]) {
				rejected_clause.push_back(dynamic_af.rejected_var(i));
			}
		}
		sat_solver->add_clause(accepted_clause);
		sat_solver->add_clause(rejected_clause);
	}

	objective_vars.clear();
	objective_vars.reserve(dynamic_af.args);
	for (uint32_t i = 0; i < dynamic_af.args; i++) {
		if (!dynamic_af.arg_exists[i]) continue;
		if (!union_of_rejected[i]) {
			objective_vars.push_back(dynamic_af.accepted_var(i));
		} else {
			vector<int> clause = { -dynamic_af.accepted_var(i) };
			if (!dynamic_af.static_mode) clause.push_back(dynamic_af.count+1);
			sat_solver->add_clause(clause);
		}
	}
	return cegar(target, (dynamic_af.static_mode ? 0 : ++dynamic_af.count));
}

void AFSolver::solve(bool cred)
{
	if (current_state == ERROR) return;
	if (dynamic_af.static_mode) {
		dynamic_af.initialize_attackers();
		dynamic_af.set_var_count();
	} else if (!dynamic_af.vars_initialized) {
		dynamic_af.add_dummy_args(buffer_size);
		buffer_size = 2*buffer_size;
		dynamic_af.vars_initialized = true;
		dynamic_af.set_var_count();
		solver_encoding = UNKNOWN_SEM;
		extensions.clear();
	}

	if (dynamic_af.static_mode || dynamic_af.sem != solver_encoding) {
		if (sat_solver) delete sat_solver;
		if (dynamic_af.sem == AD) {
			sat_solver = new SAT_Solver(dynamic_af.count, 2*dynamic_af.args);
			Encodings::add_admissible(dynamic_af, sat_solver);
			solver_encoding = AD;
		} else if (dynamic_af.sem == CO) {
			sat_solver = new SAT_Solver(dynamic_af.count, 2*dynamic_af.args);
			Encodings::add_complete(dynamic_af, sat_solver);
			solver_encoding = CO;
		} else if (dynamic_af.sem == PR) {
			sat_solver = new SAT_Solver(dynamic_af.count, 2*dynamic_af.args);
#if defined(CO_IN_PR)
			Encodings::add_complete(dynamic_af, sat_solver);
#else
			Encodings::add_admissible(dynamic_af, sat_solver);
#endif
			solver_encoding = PR;
		} else if (dynamic_af.sem == ST) {
			sat_solver = new SAT_Solver(dynamic_af.count, 2*dynamic_af.args);
			Encodings::add_stable(dynamic_af, sat_solver);
			solver_encoding = ST;
		} else if (dynamic_af.sem == SST) {
			sat_solver = new SAT_Solver(dynamic_af.count, 3*dynamic_af.args);
#if defined(CO_IN_SST)
			Encodings::add_complete(dynamic_af, sat_solver);
#else
			Encodings::add_admissible(dynamic_af, sat_solver);
#endif
			Encodings::add_range(dynamic_af, sat_solver);
			solver_encoding = SST;
		} else if (dynamic_af.sem == STG) {
			sat_solver = new SAT_Solver(dynamic_af.count, 3*dynamic_af.args);
			Encodings::add_conflict_free(dynamic_af, sat_solver);
			Encodings::add_range(dynamic_af, sat_solver);
			solver_encoding = STG;
		} else if (dynamic_af.sem == ID) {
			sat_solver = new SAT_Solver(dynamic_af.count, 2*dynamic_af.args);
#if defined(CO_IN_ID)
			Encodings::add_complete(dynamic_af, sat_solver);
#else
			Encodings::add_admissible(dynamic_af, sat_solver);
#endif
			solver_encoding = ID;
		} else {
			current_state = ERROR;
			return;
		}
	}

	int32_t target = Encodings::add_target(dynamic_af, status_assumptions, sat_solver);
	status_assumptions.clear();
	if (!cred) target = -target;
	int32_t select = (dynamic_af.static_mode ? 0 : ++dynamic_af.count);

	if (dynamic_af.sem == PR || dynamic_af.sem == SST || dynamic_af.sem == STG)
		set_objective_vars();

	if (extensions.size() > MAX_EXTENSIONS) extensions.erase(extensions.begin());
	if (dynamic_af.sem != ID && check_extensions(target, select)) {
		current_state = (cred ? ACCEPT : REJECT);
		return;
	}

	if (dynamic_af.sem == AD || dynamic_af.sem == CO) {
		assume_current_structure(sat_solver);
		if (target) sat_solver->assume(target);
		if (sat_solver->solve()) {
			current_state = (cred ? ACCEPT : REJECT);
			extensions.push_back(sat_solver->assignment);
		} else current_state = (cred ? REJECT : ACCEPT);

	} else if (dynamic_af.sem == ST) {

		assume_current_structure(sat_solver);
#if defined(GR_IN_ST)
		assume_grounded(sat_solver);
#endif
		if (target) sat_solver->assume(target);
		if (sat_solver->solve()) {
			current_state = (cred ? ACCEPT : REJECT);
			extensions.push_back(sat_solver->assignment);
		} else current_state = (cred ? REJECT : ACCEPT);

	} else if (dynamic_af.sem == PR) {

		if (cegar(target, select))
			current_state = (cred ? ACCEPT : REJECT);
		else current_state = (cred ? REJECT : ACCEPT);

	} else if (dynamic_af.sem == SST) {

#if defined(ST_EXISTS_SST)
		assume_current_structure(sat_solver);
		assume_range(sat_solver);
		if (sat_solver->solve()) {
			assume_current_structure(sat_solver);
#if defined(GR_IN_ST)
			assume_grounded(sat_solver);
#endif		
			assume_range(sat_solver);
			if (target) sat_solver->assume(target);
			if (sat_solver->solve())
				current_state = (cred ? ACCEPT : REJECT);
			else current_state = (cred ? REJECT : ACCEPT);
			return;
		}
#endif

		if (cegar(target, select))
			current_state = (cred ? ACCEPT : REJECT);
		else current_state = (cred ? REJECT : ACCEPT);	

	} else if (dynamic_af.sem == STG) {

#if defined(ST_EXISTS_STG)
		assume_current_structure(sat_solver);
		assume_range(sat_solver);
		if (sat_solver->solve()) {
			assume_current_structure(sat_solver);
#if defined(GR_IN_ST)
			assume_grounded(sat_solver);
#endif		
			assume_range(sat_solver);
			if (target) sat_solver->assume(target);
			if (sat_solver->solve())
				current_state = (cred ? ACCEPT : REJECT);
			else current_state = (cred ? REJECT : ACCEPT);
			return;
		}
#endif

		if (cegar(target, select))
			current_state = (cred ? ACCEPT : REJECT);
		else current_state = (cred ? REJECT : ACCEPT);

	} else if (dynamic_af.sem == ID) {

		if (ideal(target, select ? select : ++dynamic_af.count))
			current_state = (cred ? ACCEPT : REJECT);
		else current_state = (cred ? REJECT : ACCEPT);

	} else {
		current_state = ERROR;
	}
}

int32_t AFSolver::get_val_in(int32_t arg)
{
	if (arg < 0 || !dynamic_af.arg_to_int.count(arg)) {
		current_state = ERROR;
		return 0;
	}
	int32_t arg_index = dynamic_af.arg_to_int.at(arg);
	return sat_solver->get_value(dynamic_af.accepted_var(arg_index));
}

int32_t AFSolver::get_val_out(int32_t arg)
{
	if (arg < 0 || !dynamic_af.arg_to_int.count(arg)) {
		current_state = ERROR;
		return 0;
	}
	int32_t arg_index = dynamic_af.arg_to_int.at(arg);
	return sat_solver->get_value(dynamic_af.rejected_var(arg_index));
}
