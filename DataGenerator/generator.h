#include <windows.h>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include "..\findit\utils.h"
#include "..\findit\string.h"
#pragma once

using namespace std;

#ifdef NDEBUG
default_random_engine engine(time(nullptr));
#else
static default_random_engine engine;
#endif

static const uniform_real_distribution<double> probability(0.0, 1.0);

class Generator {
private:
	char *raw, *corrupted;
public:
	vector<vector<int>> blanks;
	vector<String> patterns, queries;
	int current;

	Generator() = delete;
	Generator(Generator &g) = delete;
	Generator(const char *path, int min_cnt = 100, int max_cnt = 300, double noise = 0.2);
	template <class Distribution>
	void BuildPatterns(int samples, Distribution length_gen, bool append = false, double negative = 0.1);
	void Generate(FILE* fout, int insert, int remove, int query);
};

#pragma region Generator

template <class Distribution>
void Generator::BuildPatterns(int samples, Distribution length_gen, bool append, double negative)
{
	uniform_int_distribution<int> line_gen(0, blanks.size() - 1), blank_gen;
	char *source;

	if (!append)
		patterns.clear();
	while (samples--) {
		if (probability(engine) >= negative)
			source = raw;
		else
			source = corrupted;
		int len = max((int)length_gen(engine), 1), line;
		do {
			line = line_gen(engine);
		} while (blanks[line].size() <= len);
		blank_gen = uniform_int_distribution<int>(0, blanks[line].size() - len - 1);
		int b = blank_gen(engine);
		int start = blanks[line][b], end = blanks[line][b + len];
		patterns.push_back(String((unsigned char *)source + start + 1, end - start - 1));
	}
}

#pragma endregion