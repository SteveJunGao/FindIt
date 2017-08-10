#include <unordered_map>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_unordered_map.h>
#include <atomic>
#include <thread>
#include <mutex>
#include "hash.h"
#pragma once

class Solver {
public:
	virtual vector<vector<String>> RunBatch(const vector<String> &batch) = 0;
};

class NaiveSerial : public Solver {
public:
	unordered_map<DoubleHashValue, String> table;

	void Insert(const String &pattern);
	void Remove(const String &pattern);
	vector<String> Query(const String &text);
	vector<vector<String>> RunBatch(const vector<String> &batch);
};

class NaiveParallel : public Solver {
public:
	unordered_map<DoubleHashValue, String> table;

	int thread_cnt;
	vector<thread> workers;

	// parallel
	atomic<size_t> task_idx, sub_idx;
	mutex mtx;
	vector<DoubleHashValue> hashs; // no need to use concurrent_vector as threads write different slots
	tbb::concurrent_vector<pair<int, int>> occurs;

	NaiveParallel() = delete;
	NaiveParallel(int thread_cnt) : thread_cnt(thread_cnt) { workers.resize(thread_cnt); }

	void Insert(DoubleHashValue key, const String &pattern);
	void Remove(DoubleHashValue key);
	vector<String> Query(const String &text);
	vector<vector<String>> RunBatch(const vector<String> &batch);

	void HashWorker(const vector<String> &batch);
	void QueryWorker(const String &text, const StringHash &hash);
};

class OpenMPParallel : public Solver {
public:
	unordered_map<DoubleHashValue, String> table;

	int thread_cnt;

	// parallel
	vector<DoubleHashValue> hashs;

	OpenMPParallel() = delete;
	OpenMPParallel(int thread_cnt) : thread_cnt(thread_cnt) {}

	void Insert(DoubleHashValue key, const String &pattern);
	void Remove(DoubleHashValue key);
	vector<String> Query(const String & text);
	vector<vector<String>> RunBatch(const vector<String> & batch);
};

class Timestamp {
public:
	bool exist;
	int batch_id, line_id;

	Timestamp();
	Timestamp(bool exist, int batch_id, int line_id) : exist(exist), batch_id(batch_id), line_id(line_id) {}

	inline bool operator <(const Timestamp &a)
	{
		return (batch_id < a.batch_id) || (batch_id == a.batch_id) && (line_id < a.line_id);
	}
};

class PowerfulParallel : public Solver {
public:	
	tbb::concurrent_unordered_map<DoubleHashValue, // first hash
		unordered_map<int, // key gram count
			unordered_map<DoubleHashValue, // key hash
				unordered_map<int, // full length
					unordered_map<DoubleHashValue, // full hash
						vector<Timestamp>>>>>> table;

	int thread_cnt;
	vector<thread> workers;

	// parallel
	int batch_id;
	atomic<size_t> task_idx;
	// vector<atomic<size_t>> sub_idx; // atomic is not copy-assignable
	vector<size_t> sub_idx;
	mutex mtx;
	tbb::concurrent_vector<pair<Timestamp, GramHash>> queries;
	tbb::concurrent_vector<vector<pair<int, int>>> occurs;
	static const int chunk_size;

	PowerfulParallel() = delete;
	PowerfulParallel(int thread_cnt) : thread_cnt(thread_cnt) { workers.resize(thread_cnt); }

	vector<vector<String>> RunBatch(const vector<String> &batch);

	vector<String> _batch;

	void HashWorker(const vector<String> &batch);
	void QueryWorker();
};