#include <algorithm>
#include "solver.h"

using namespace std;

#pragma region NaiveSerial

vector<vector<String>> NaiveSerial::RunBatch(const vector<String> &batch)
{
	vector<vector<String>> results;

	for (size_t i = 0; i < batch.size(); i++)
		switch (batch[i].s[0]) {
			case 'A': Insert(batch[i] + 1); break;
			case 'D': Remove(batch[i] + 1); break;
			case 'Q': results.push_back(Query(batch[i] + 1)); break;
		}
	return results;
}

void NaiveSerial::Insert(const String &pattern)
{
	table[StringHash(pattern)] = pattern;
}

void NaiveSerial::Remove(const String &pattern)
{
	table.erase(StringHash(pattern));
}

vector<String> NaiveSerial::Query(const String &text)
{
	vector<pair<int, int>> occurs;
	vector<String> result;
	StringHash hash = StringHash(text);

	for (int l = 0; l < text.length; l++)
		if (text.s[l] == ' ') {
			for (int r = l + 1; r <= text.length; r++)
				if (text.s[r - 1] == ' ')
					if (table.find(hash(l, r)) != table.end())
						occurs.push_back(make_pair(l, r - l));
		}
	sort(occurs.begin(), occurs.end());
	for (size_t i = 0; i < occurs.size(); i++)
		result.push_back(String(text.s + occurs[i].first, occurs[i].second));
	return result;
}

#pragma endregion