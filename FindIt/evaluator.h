#include <windows.h>
#include "utils.h"
#include "solver.h"
#pragma once

const int MAX_LINE_LEN = 128 << 20; // MB

class Evaluator {
public:
	Solver * const solver;

	Evaluator() = delete;
	Evaluator(Solver *solver) : solver(solver) {}
	void Run(const char *input_file, const char *output_file, const char *answer_file);
};