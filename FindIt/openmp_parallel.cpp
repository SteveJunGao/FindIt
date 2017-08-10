#include <algorithm>
#include "solver.h"

using namespace std;

#pragma region OpenMPParallel

vector<vector<String>> OpenMPParallel::RunBatch(const vector<String> &batch)
{
	// Using openmp to parallel hashing
	// Parallel Hashing
	hashs.resize(batch.size());
	#pragma omp parallel num_threads(thread_cnt)
	{
	#pragma omp for schedule(dynamic)
		for (int i = 0; i < batch.size(); i++){
			if (batch[i].s[0] != 'Q')
				hashs[i] = StringHash(batch[i] + 1);
		}
	}
	vector<vector<String>> results;
	for (int i = 0; i < batch.size(); i++)
		switch (batch[i].s[0]) {
		case 'A': Insert(hashs[i], batch[i] + 1); break;
		case 'D': Remove(hashs[i]); break;
		case 'Q': results.push_back(Query(batch[i] + 1)); break;
	}
	return results;
}

void OpenMPParallel::Insert(DoubleHashValue key, const String& pattern)
{
	table[key] = pattern;
}

void OpenMPParallel::Remove(DoubleHashValue key)
{
	table.erase(key);
}


vector<String> OpenMPParallel::Query(const String & text)
{
	vector<pair<int, int>> occurs;
	vector<String> results;
	StringHash hash = StringHash(text);
	int l = 0;
	#pragma omp parallel num_threads(thread_cnt)
	{
	#pragma omp for schedule(dynamic)
		for (l = 0; l < text.length; l++){
			if (text.s[l] == ' ') {
				for (int r = l + 1; r <= text.length; r++)
					if (text.s[r - 1] == ' ')
						if (table.find(hash(l, r)) != table.end())
							#pragma omp critical
							occurs.push_back(make_pair(l, r - l));
			}
		} 
	}
	sort(occurs.begin(), occurs.end());
	//printf("Length find %d\n", occurs.size());
	for (int i = 0; i < occurs.size(); i++)
		results.push_back(String(text.s + occurs[i].first, occurs[i].second));
	return results;
}

#pragma endregion