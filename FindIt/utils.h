#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <typeinfo>
#pragma once

#ifndef NDEBUG
	#define debug(...) printf(__VA_ARGS__)
#else
	#define debug
#endif

#define panic(format, ...) do { \
	printf("%s, line %d: " ## format ## "\n", __FILE__, __LINE__, __VA_ARGS__); \
	system("pause"); \
	exit(-1); \
} while (0)

union UInt64 {
	unsigned long part[2];
	unsigned long long all;
};