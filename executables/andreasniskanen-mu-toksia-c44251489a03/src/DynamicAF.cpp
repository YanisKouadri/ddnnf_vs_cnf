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

#include "DynamicAF.h"

using namespace std;

DynamicAF::DynamicAF() : args(0), args_remaining(0), count(0) {
#if defined(DYNAMIC_ASSUMPS)
	static_mode = false;
#else
	static_mode = true;
#endif
	vars_initialized = false;
}

bool DynamicAF::add_argument(int32_t arg)
{
	if (arg < 0) return false;
	if (arg_to_int.count(arg) > 0) {
		return false;
	}
	int_to_arg.push_back(arg);
	if (args_remaining == 0) {
		arg_to_int[arg] = args++;
		arg_exists.push_back(true);
	} else {
		args_remaining--;
		arg_to_int[arg] = args - args_remaining - 1;
		arg_exists[arg_to_int[arg]] = true;
	}
	if (args > MAX_DYNAMIC_ARGS)
		static_mode = true;
	return true;
}

bool DynamicAF::del_argument(int32_t arg)
{
	if (arg < 0) return false;
	if (arg_to_int.count(arg) == 0) {
		return false;
	}
	int32_t arg_index = arg_to_int[arg];
	arg_exists[arg_index] = false;
	arg_to_int.erase(arg);
	for (uint32_t i = 0; i < args; i++) {
		if (attacks.count(make_pair(arg_index, i)))
			attacks.erase(make_pair(arg_index, i));
		if (attacks.count(make_pair(i, arg_index)))
			attacks.erase(make_pair(i, arg_index));
	}
	return true;
}

bool DynamicAF::add_attack(int32_t source, int32_t target)
{
	if (source < 0 || target < 0) return false;
	if (arg_to_int.count(source) == 0 || arg_to_int.count(target) == 0) {
		return false;
	}
	int32_t source_index = arg_to_int[source];
	int32_t target_index = arg_to_int[target];
	if (!arg_exists[source_index] || !arg_exists[target_index]) {
		return false;
	}
	if (attacks.count(make_pair(source_index, target_index))) {
		return false;
	}
	attacks.insert(make_pair(source_index, target_index));
	return true;
}

bool DynamicAF::del_attack(int32_t source, int32_t target)
{
	if (source < 0 || target < 0) return false;
	if (arg_to_int.count(source) == 0 || arg_to_int.count(target) == 0) {
		return false;
	}
	int32_t source_index = arg_to_int[source];
	int32_t target_index = arg_to_int[target];
	if (!arg_exists[source_index] || !arg_exists[target_index]) {
		return false;
	}
	if (!attacks.count(make_pair(source_index, target_index))) {
		return false;
	}
	attacks.erase(make_pair(source_index, target_index));
	return true;
}

void DynamicAF::add_dummy_args(int32_t n_args) {
	args_remaining = n_args;
	for (int32_t i = 0; i < n_args; i++) {
		args++;
		arg_exists.push_back(false);
	}
}

void DynamicAF::initialize_attackers()
{
	if (!static_mode) return;
	attackers.clear();
	self_attack.clear();
	symmetric_attacks.clear();
	attackers.resize(args);
	self_attack.resize(args);
	for (const pair<int32_t,int32_t> & attack : attacks) {
		int32_t source = attack.first;
		int32_t target = attack.second;
		attackers[target].push_back(source);
		if (source == target)
			self_attack[source] = true;
		if (attacks.count(make_pair(target, source))) {
			symmetric_attacks.insert(make_pair(source, target));
			symmetric_attacks.insert(make_pair(target, source));
		}
	}
}

void DynamicAF::set_var_count()
{
	if (static_mode) {
		if (sem != SST && sem != STG)
			count = 2*args;
		else
			count = 3*args;
	} else {
		count = 4*args + 3*args*args;
	}
}