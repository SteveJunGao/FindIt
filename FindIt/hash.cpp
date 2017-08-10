#include "hash.h"

#pragma region GramHash

GramHash::GramHash(String s) : StringHash(s)
{
	for (int i = 0; i < s.length; i++)
		if (s.s[i] == ' ')
			blanks.push_back(i);
	gram_count = blanks.size() - 1;
	for (key_gram_count = 1; key_gram_count <= gram_count / 2; key_gram_count *= 2);
	full_length = prefixs[0].size() - 1;
	first_hash = this->operator()(0, 1);
	key_hash = this->operator()(0, key_gram_count);
	full_hash = *this;
}

DoubleHashValue GramHash::operator()(int l, int r) const
{
	return this->StringHash::operator()(blanks[l], blanks[r] + 1);
}

#pragma endregion