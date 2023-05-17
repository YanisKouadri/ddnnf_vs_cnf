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

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <getopt.h>
#include <chrono>
#include <iomanip>

#include <unistd.h>
#include <stdio.h>

using namespace std;

static int version_flag = 0;
static int usage_flag = 0;
static int formats_flag = 0;
static int problems_flag = 0;

task string_to_task(string problem)
{
	string tmp = problem.substr(0, problem.find("-"));
	if (tmp == "DC") return DC;
	if (tmp == "DS") return DS;
	if (tmp == "SE") return SE;
	return UNKNOWN_TASK;
}

sigma string_to_sem(string problem)
{
	problem.erase(0, problem.find("-") + 1);
	string tmp = problem.substr(0, problem.find("-"));
	if (tmp == "AD") return AD;
	if (tmp == "CO") return CO;
	if (tmp == "PR") return PR;
	if (tmp == "ST") return ST;
	if (tmp == "SST") return SST;
	if (tmp == "STG") return STG;
	if (tmp == "ID") return ID;
	return UNKNOWN_SEM;
}

void print_usage(string solver_name)
{
	cout << "Usage: " << solver_name << " -p <task> -f <file> [-fo <format>] [-a <query>]\n\n";
	cout << "  <task>      computational problem; for a list of available problems use option --problems\n";
	cout << "  <file>      input argumentation framework\n";
	cout << "  <format>    file format for input AF; for a list of available formats use option --formats\n";
	cout << "  <query>     query argument\n";
	cout << "Options:\n";
	cout << "  --help      Displays this help message.\n";
	cout << "  --version   Prints version and author information.\n";
	cout << "  --formats   Prints available file formats.\n";
	cout << "  --problems  Prints available computational tasks.\n";
}

void print_version(string solver_name)
{
	cout << solver_name << " (version ICCMA'23)\n" << "Andreas Niskanen, University of Helsinki <andreas.niskanen@helsinki.fi>\n";
}

void print_formats()
{
	cout << "[iccma23,apx,tgf]" << endl;
}

void print_problems()
{
	vector<string> tasks = {"DC","DS","SE"};
	vector<string> sems = {"AD","CO","PR","ST","SST","STG","ID"};
	cout << "[";
	for (uint32_t i = 0; i < tasks.size(); i++) {
		for (uint32_t j = 0; j < sems.size(); j++) {
			string problem = tasks[i] + "-" + sems[j];
			if (problem != "SE-ID") cout << problem << ",";
			else cout << problem;
		}
	}
	cout << "]" << endl;
}

int main(int argc, char ** argv)
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	if (argc == 1) {
		print_version(argv[0]);
		return 0;
	}

	const struct option longopts[] =
	{
		{"help", no_argument, &usage_flag, 1},
		{"version", no_argument, &version_flag, 1},
		{"formats", no_argument, &formats_flag, 1},
		{"problems", no_argument, &problems_flag, 1},
		{"p", required_argument, 0, 'p'},
		{"f", required_argument, 0, 'f'},
		{"fo", required_argument, 0, 'o'},
		{"a", required_argument, 0, 'a'},
		{0, 0, 0, 0}
	};

	int option_index = 0;
	int opt = 0;
	string task, file, fileformat, query;

	while ((opt = getopt_long_only(argc, argv, "", longopts, &option_index)) != -1) {
		switch (opt) {
			case 0:
				break;
			case 'p':
				task = optarg;
				break;
			case 'f':
				file = optarg;
				break;
			case 'o':
				fileformat = optarg;
				break;
			case 'a':
				query = optarg;
				break;
			default:
				return 1;
		}
	}

	if (version_flag) {
		print_version(argv[0]);
		return 0;
	}

	if (usage_flag) {
		print_usage(argv[0]);
		return 0;
	}

	if (formats_flag) {
		print_formats();
		return 0;
	}

	if (problems_flag) {
		print_problems();
		return 0;
	}

	if (task.empty()) {
		cerr << argv[0] << ": Task must be specified via -p flag\n";
		return 1;
	}

	if (file.empty()) {
		cerr << argv[0] << ": Input file must be specified via -f flag\n";
		return 1;
	}

	ifstream input;
	input.open(file);

	if (!input.good()) {
		cerr << argv[0] << ": Cannot open input file\n";
		return 1;
	}

	AFSolver solver = AFSolver();
	solver.set_static();
	solver.set_semantics(string_to_sem(task));
	string line, arg, source, target;
	int32_t n_args = 0;
	unordered_map<string,int32_t> arg_str_to_int;
	unordered_map<int32_t,string> int_to_arg_str;

	if (fileformat.empty()) {
		while (!input.eof()) {
			getline(input, line);
			if (line.length() == 0 || line[0] == '#') continue;
			std::istringstream iss(line);
			if (line[0] == 'p') {
				string p, af;
				iss >> p >> af >> n_args;
				for (int32_t i = 1; i <= n_args; i++) {
					solver.add_argument(i);
				}
			} else {
				int32_t s, t;
				iss >> s >> t;
				solver.add_attack(s,t);
			}
		}
	} else if (fileformat == "apx") {
		while (!input.eof()) {
			getline(input, line);
			line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
			if (line.length() == 0 || line[0] == '/' || line[0] == '%') continue;
			if (line.length() < 4) cerr << "Warning: Cannot parse line: " << line << "\n";
			string op = line.substr(0,3);
			if (op == "arg" && line[3] == '(' && line.find(')') != string::npos) {
				arg = line.substr(4,line.find(')')-4);
				arg_str_to_int[arg] = ++n_args;
				int_to_arg_str[n_args] = arg;
				solver.add_argument(arg_str_to_int[arg]);
			} else if (op == "att" && line[3] == '(' && line.find(',') != string::npos && line.find(')') != string::npos) {
				source = line.substr(4,line.find(',')-4);
				target = line.substr(line.find(',')+1,line.find(')')-line.find(',')-1);
				solver.add_attack(arg_str_to_int[source], arg_str_to_int[target]);
			} else {
				cerr << "Warning: Cannot parse line: " << line << "\n";
			}
		}
	} else if (fileformat == "tgf") {
		while (!input.eof()) {
			getline(input, line);
			std::istringstream iss(line);
			iss >> arg;
			if (arg == "#") break;
			arg_str_to_int[arg] = ++n_args;
			int_to_arg_str[n_args] = arg;
			solver.add_argument(arg_str_to_int[arg]);
		}
		while (!input.eof()) {
			getline(input, line);
			if (line.empty()) break;
			std::istringstream iss(line);
			iss >> source >> target;
			solver.add_attack(arg_str_to_int[source], arg_str_to_int[target]);
		}
	} else {
		cerr << argv[0] << ": Unsupported file format\n";
		return 1;
	}

	input.close();
	auto start = std::chrono::high_resolution_clock::now();
	int32_t q;
	switch (string_to_task(task)) {

		case DC:
			if (query.empty()) {
				cerr << argv[0] << ": Query argument must be specified via -a flag\n";
				return 1;
			}
			q = (fileformat.empty() ? stoi(query) : arg_str_to_int[query]);
			solver.assume_in(q);
			solver.solve(true);
			if (solver.get_state() == ACCEPT) {
				cout << "YES" << endl;
				cout << "w ";
				for (int32_t i = 1; i <= n_args; i++) {
					if (solver.get_val_in(i) > 0)
						cout << (fileformat.empty() ? to_string(i) : int_to_arg_str[i]) << " ";
				}
				cout << endl;
			} else if (solver.get_state() == REJECT) {
				cout << "NO" << endl;
			} else {
				return 1;
			}
			break;

		case DS:
			if (query.empty()) {
				cerr << argv[0] << ": Query argument must be specified via -a flag\n";
				return 1;
			}
			q = (fileformat.empty() ? stoi(query) : arg_str_to_int[query]);
			solver.assume_in(q);
			solver.solve(false);
			if (solver.get_state() == ACCEPT) {
				cout << "YES" << endl;
			} else if (solver.get_state() == REJECT) {
				cout << "NO" << endl;
				cout << "w ";
				for (int32_t i = 1; i <= n_args; i++) {
					if (solver.get_val_in(i) > 0)
						cout << (fileformat.empty() ? to_string(i) : int_to_arg_str[i]) << " ";
				}
				cout << endl;
			} else {
				return 1;
			}
			break;

		case SE:
			solver.solve(true);
			if (solver.get_state() == ACCEPT) {
				cout << "w ";
				for (int32_t i = 1; i <= n_args; i++) {
					if (solver.get_val_in(i) > 0)
						cout << (fileformat.empty() ? to_string(i) : int_to_arg_str[i]) << " ";
				}
				cout << endl;
			} else if (solver.get_state() == REJECT) {
				cout << "NO" << endl;
			} else {
				return 1;
			}
			break;

		default:
			cerr << argv[0] << ": Problem not supported!\n";
			return 1;

	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration<double>(end - start).count();
	if(string_to_task(task)==DC){
		std::ofstream output("benchmarks_DC.csv", std::ios_base::app);
		output << "," << std::setprecision(16) << std::fixed << duration;
		output.close();
 }
 else if(string_to_task(task)==DS){
  std::ofstream output("benchmarks_DS.csv", std::ios_base::app);
		output << "," << std::setprecision(16) << std::fixed << duration;
		output.close();
 }
 else{
  cout << "mu-toksia : problem with the benchmark csv writing, the task should be DC or DS" << endl;
 }
	return 0;
}