#include <algorithm>
#include "solver.h"

#pragma region PowerfulParallel

const int PowerfulParallel::chunk_size = 16;

vector<vector<String>> PowerfulParallel::RunBatch(const vector<String> &batch)
{
	vector<vector<String>> results;

	batch_id++;
	task_idx = 0;
	queries.clear();
	for (int i = 0; i < thread_cnt; i++)
		workers[i] = thread(&PowerfulParallel::HashWorker, this, batch);
	for (int i = 0; i < thread_cnt; i++)
		workers[i].join();
	typedef pair<Timestamp, GramHash> Query;
	sort(queries.begin(), queries.end(), [&](Query a, Query b){ return a.first < b.first; });

	_batch = batch;
	task_idx = 0;
	sub_idx.clear();
	sub_idx.resize(queries.size());
	occurs.clear();
	occurs.resize(queries.size());
	for (int i = 0; i < thread_cnt; i++)
		workers[i] = thread(&PowerfulParallel::QueryWorker, this);
	for (int i = 0; i < thread_cnt; i++)
		workers[i].join();
	for (size_t i = 0; i < queries.size(); i++) {
		String text = batch[queries[i].first.line_id] + 1;
		sort(occurs[i].begin(), occurs[i].end());
		vector<String> result;
		for (size_t j = 0; j < occurs[i].size(); j++)
			result.push_back(String(text.s + occurs[i][j].first, occurs[i][j].second));
		results.push_back(result);
	}

	return results;
}

void PowerfulParallel::HashWorker(const vector<String> &batch)
{
	size_t i;
	while ((i = task_idx.fetch_add(1)) < batch.size()) {
		GramHash hash = GramHash(batch[i] + 1);
		Timestamp ts = Timestamp(batch[i].s[0] == 'A', batch_id, i);
		switch (batch[i].s[0]) {
		case 'A': case 'D': {
			// without mutex it fails with runtime error here, don't know why
			lock_guard<mutex> guard(mtx);
			table[hash.first_hash]
				[hash.key_gram_count]
				[hash.key_hash]
				[hash.full_length]
				[hash.full_hash].push_back(ts);
		}
			break;
		case 'Q':
			queries.push_back(make_pair(ts, hash));
		}
	}
}

void PowerfulParallel::QueryWorker()
{
	size_t i, j;
	while ((i = task_idx.fetch_add(1)) < queries.size()) {
		Timestamp &ts = queries[i].first;
		GramHash &hash = queries[i].second;
		while ((j = sub_idx[i]) < hash.gram_count) {
			sub_idx[i] += chunk_size;
			for (size_t _j = j; _j < j + chunk_size && _j < hash.gram_count; _j++) {
				DoubleHashValue first_expected = hash(_j, _j + 1);
				if (table.find(first_expected) == table.end())
					continue;
				auto &key_gram_table = table[first_expected];
				for (auto p = key_gram_table.begin(); p != key_gram_table.end(); p++) {
					int key_gram_count = p->first;
					if (_j + key_gram_count > hash.gram_count)
						continue;
					DoubleHashValue key_expected = hash(_j, _j + key_gram_count);
					auto &key_table = p->second;
					if (key_table.find(key_expected) == key_table.end())
						continue;
					auto &full_length_table = key_table[key_expected];
					for (auto q = full_length_table.begin(); q != full_length_table.end(); q++) {
						int full_length = q->first, l = hash.blanks[_j];
						DoubleHashValue full_expected = ((StringHash)hash)(l, l + full_length);
						auto &full_table = q->second;
						if (full_table.find(full_expected) == full_table.end())
							continue;
						vector<Timestamp> &tslist = full_table[full_expected];
						size_t k;
						for (k = 0; k < tslist.size(); k++)
							if (ts < tslist[k])
								break;
						if (tslist[--k].exist)
							occurs[i].push_back(make_pair(l, full_length));
					}
				}
			}
		}
	}
}

#pragma endregion