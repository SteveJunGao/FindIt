#include <assert.h>
#pragma once

class String {
public:
	const unsigned char *s;
	int length;

	String() {}
	String(const unsigned char *s, int length) : s((const unsigned char *) s), length(length)
	{ assert(length >= 0); }

	inline operator bool()
	{
		return (length > 0);
	}

	inline friend bool operator <(const String &a, const String &b)
	{
		return a.s < b.s || (a.s == b.s && a.length < b.length);
	}

	inline bool operator ==(const String &a) const
	{
		return s == a.s && length == a.length;
	}

	inline String operator +(int n) const
	{
		return String(s + n, length - n);
	}

	inline String operator -(int n) const
	{
		return String(s - n, length + n);
	}

	inline int operator -(String &a)
	{
		assert(s + length == a.s + a.length);
		return s - a.s;
	}

	inline String &operator +=(int n)
	{
		s += n;
		length -= n;
		return *this;
	}

	inline String &operator -=(int n)
	{
		s -= n;
		length += n;
		return *this;
	}

	inline String &operator ++()
	{
		return *this += 1;
	}

	inline String &operator --()
	{
		return *this -= 1;
	}
};

inline int strncmp(const unsigned char *a, const unsigned char *b, size_t s)
{
	return strncmp((char *)a, (char *)b, s);
}

inline int strncmp(const char *a, const unsigned char *b, size_t s)
{
	return strncmp(a, (char *)b, s);
}