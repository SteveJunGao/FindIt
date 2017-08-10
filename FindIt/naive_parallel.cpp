#include <algorithm>
#include "solver.h"

#pragma region NaiveParallel

vector<vector<String>> NaiveParallel::RunBatch(const vector<String> &batch)
{
	vector<vector<String>> results;

	hashs.resize(batch.size());
	task_idx = 0;
	for (int i = 0; i < thread_cnt; i++)
		workers[i] = thread(&NaiveParallel::HashWorker, this, batch);
	for (int i = 0; i < thread_cnt; i++)
		workers[i].join();

	for (size_t i = 0; i < batch.size(); i++)
		switch (batch[i].s[0]) {
		case 'A': Insert(hashs[i], batch[i] + 1); break;
		case 'D': Remove(hashs[i]); break;
		case 'Q': results.push_back(Query(batch[i] + 1)); break;
	}
	return results;
}

void NaiveParallel::Insert(DoubleHashValue key, const String& pattern)
{
	table[key] = pattern;
}

void NaiveParallel::Remove(DoubleHashValue key)
{
	table.erase(key);
}

vector<String> NaiveParallel::Query(const String &text)
{
	vector<String> result;
	StringHash hash = StringHash(text);

	occurs.clear();
	sub_idx = 0;
	for (int i = 0; i < thread_cnt; i++)
		workers[i] = thread(&NaiveParallel::QueryWorker, this, text, hash);
	for (int i = 0; i < thread_cnt; i++)
		workers[i].join();
	
	sort(occurs.begin(), occurs.end());
	for (size_t i = 0; i < occurs.size(); i++)
		result.push_back(String(text.s + occurs[i].first, occurs[i].second));
	return result;
}

void NaiveParallel::HashWorker(const vector<String> &batch)
{
	size_t i;
	while ((i = task_idx.fetch_add(1)) < batch.size()) {
		if (batch[i].s[0] != 'Q')
			hashs[i] = StringHash(batch[i] + 1);
	}
}

void NaiveParallel::QueryWorker(const String &text, const StringHash &hash)
{
	int length = hash.prefixs[0].size() - 1, l;
	while ((l = sub_idx.fetch_add(1)) < length) {
		if (text.s[l] == ' ')
			for (int r = l + 1; r <= length; r++)
				if (text.s[r - 1] == ' ')
					if (table.find(hash(l, r)) != table.end())
						occurs.push_back(make_pair(l, r - l));
	}
}

#pragma endregion