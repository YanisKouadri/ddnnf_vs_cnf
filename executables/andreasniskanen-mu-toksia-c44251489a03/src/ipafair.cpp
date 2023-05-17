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

extern "C" {

#include "ipafair.h"

static AFSolver * import (void * s) { return (AFSolver *) s; }

const char * ipafair_signature () { return "μ-toksia"; }
void * ipafair_init () { return new AFSolver(); }
void ipafair_release (void * s) { delete import(s); }
void ipafair_set_semantics (void * s, semantics sem) { import(s)->set_semantics((sigma)sem); }
void ipafair_add_argument (void * s, int32_t a) { import(s)->add_argument(a); }
void ipafair_del_argument (void * s, int32_t a) { import(s)->del_argument(a); }
void ipafair_add_attack (void * s, int32_t a, int32_t b) { import(s)->add_attack(a,b); }
void ipafair_del_attack (void * s, int32_t a, int32_t b) { import(s)->del_attack(a,b); }
void ipafair_assume (void * s, int32_t a) { import(s)->assume_in(a); }
int ipafair_solve_cred (void * s) { import(s)->solve(true); return import(s)->get_state(); }
int ipafair_solve_skept (void * s) { import(s)->solve(false); return import(s)->get_state(); }
int32_t ipafair_val (void * s, int32_t a) { return import(s)->get_val_in(a); }

};