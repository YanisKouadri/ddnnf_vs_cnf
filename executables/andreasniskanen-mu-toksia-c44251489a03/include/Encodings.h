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

#ifndef ENCODINGS_H
#define ENCODINGS_H

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

namespace Encodings {

void add_conflict_free(const DynamicAF & af, SAT_Solver * solver);
void add_admissible(const DynamicAF & af, SAT_Solver * solver);
void add_complete(const DynamicAF & af, SAT_Solver * solver);
void add_stable(const DynamicAF & af, SAT_Solver * solver);
void add_range(const DynamicAF & af, SAT_Solver * solver);
int32_t add_target(DynamicAF & af, const std::vector<int32_t> & assumptions, SAT_Solver * solver);

}

#endif