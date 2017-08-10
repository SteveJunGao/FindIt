#include <vector>
#include "utils.h"
#include "string.h"
#pragma once

using namespace std;

typedef unsigned long HashValue;
typedef unsigned long long DoubleHashValue;

template <int base0 = 131, int base1 = 31313, int max_len = 100000>
class _StringHash {
public:
	static const int base[2]; // for coding convenience
	static vector<HashValue> exps[2];
	vector<HashValue> prefixs[2];

	_StringHash() = delete;
	_StringHash(String s);
	DoubleHashValue operator()(int l, int r) const; // [l, r)
	operator DoubleHashValue();
};

typedef _StringHash<> StringHash;

class GramHash : public StringHash {
public:
	int gram_count, key_gram_count, full_length;
	vector<int> blanks;

	DoubleHashValue first_hash, key_hash, full_hash;

	GramHash() = delete;
	GramHash(String s);
	DoubleHashValue GramHash::operator()(int l, int r) const;
};

#pragma region _StringHash

// explicit declare static members
template <int base0, int base1, int max_len>
const int _StringHash<base0, base1, max_len>::base[2] = { base0, base1 };

template <int base0, int base1, int max_len>
vector<HashValue> _StringHash<base0, base1, max_len>::exps[2];

template <int base0, int base1, int max_len>
_StringHash<base0, base1, max_len>::_StringHash(String s)
{
	assert(s.length <= max_len);
	if (exps[0].empty()) {
		for (int j = 0; j < 2; j++) {
			exps[j].resize(max_len + 1);
			exps[j][0] = 1;
			for (int i = 0; i < max_len; i++)
				exps[j][i + 1] = exps[j][i] * base[j];
		}
	}
	for (int j = 0; j < 2; j++) {
		prefixs[j].resize(s.length + 1);
		for (int i = 0; i < s.length; i++)
			prefixs[j][i + 1] = prefixs[j][i] * base[j] + s.s[i];
	}
}

template <int base0, int base1, int max_len>
DoubleHashValue _StringHash<base0, base1, max_len>::operator()(int l, int r) const
{
	UInt64 ret;
	
	for (int j = 0; j < 2; j++)
		ret.part[j] = prefixs[j][r] - prefixs[j][l] * exps[j][r - l];
	return ret.all;
}

template <int base0, int base1, int max_len>
_StringHash<base0, base1, max_len>::operator DoubleHashValue()
{
	return this->operator()(0, prefixs[0].size() - 1);
}

#pragma endregion