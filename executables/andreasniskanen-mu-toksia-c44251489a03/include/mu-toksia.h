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

#define INITIAL_BUFFER_SIZE 16
#define MAX_EXTENSIONS 8

#ifndef AF_SOLVER_H
#define AF_SOLVER_H

#include "DynamicAF.h"

#if defined(SAT_GLUCOSE)
#include "GlucoseSolver.h"
typedef GlucoseSolver SAT_Solver;
#elif defined(SAT_CMSAT)
#include "CryptoMiniSatSolver.h"
typedef CryptoMiniSatSolver SAT_Solver;
#else
#error "No SAT solver defined"
#endif

enum state { INPUT = 0, ACCEPT = 10, REJECT = 20, ERROR = -1 };

class AFSolver {

public:
	AFSolver();
	~AFSolver() { if (sat_solver) delete sat_solver; };
	void set_semantics(sigma s);
	void add_argument(int32_t arg);
	void del_argument(int32_t arg);
	void add_attack(int32_t source, int32_t target);
	void del_attack(int32_t source, int32_t target);
	void assume_in(int32_t arg);
	void assume_out(int32_t arg);
	void solve(bool cred);
	int32_t get_val_in(int32_t arg);
	int32_t get_val_out(int32_t arg);
	state get_state() { return current_state; }
	void set_static() { dynamic_af.static_mode = true; }

private:
	DynamicAF dynamic_af;
	SAT_Solver * sat_solver;
	sigma solver_encoding;
	std::vector<int32_t> extension;
	std::vector<int32_t> status_assumptions;
	std::vector<int32_t> objective_vars;
	std::vector<std::vector<uint8_t>> extensions;
	state current_state;
	uint32_t buffer_size;

	void assume_current_structure(SAT_Solver * solver);
	void assume_grounded(SAT_Solver * solver);
	void assume_range(SAT_Solver * solver);
	void set_objective_vars();
	bool check_extensions(int32_t target, int32_t select);
	bool cegar(int32_t target, int32_t select);
	bool ideal(int32_t target, int32_t select);

};

#endif