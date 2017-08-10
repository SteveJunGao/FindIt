#include "generator.h"

int main()
{
	Generator gen(R"(..\data\Homer\)", 200, 2000);
	gen.BuildPatterns(1000000, uniform_int_distribution<int>(10, 50), true);
	
	FILE *fout = fopen(R"(..\data\Homer.input)", "w");
	for (int i = 0; i < 100; i++) // batch
		gen.Generate(fout, 1000, 100, 100);
	fclose(fout);
}