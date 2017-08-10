#include "solver.h"
#include "evaluator.h"

int main()
{
	NaiveSerial naive_serial;
	Evaluator evaluator1(&naive_serial);
	evaluator1.Run(R"(..\data\Homer.input)", R"(..\data\Homer.answer)", NULL);

	NaiveParallel naive_parallel(8);
	Evaluator evaluator2(&naive_parallel);
	evaluator2.Run(R"(..\data\Homer.input)", R"(..\data\Homer.output)", R"(..\data\Homer.answer)");

	OpenMPParallel openmp_parallel(8);
	Evaluator evaluator3(&openmp_parallel);
	evaluator3.Run(R"(..\data\Homer.input)", R"(..\data\Homer.output)", R"(..\data\Homer.answer)");

	PowerfulParallel parallel(8);
	Evaluator evaluator4(&parallel);
	evaluator4.Run(R"(..\data\Homer.input)", R"(..\data\Homer.output)", R"(..\data\Homer.answer)");

	scanf("\n");
}