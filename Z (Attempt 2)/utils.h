#pragma once

#include <iostream>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define ARR_LEN(arr) \
	sizeof(arr) / sizeof(arr[0])

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ANSI codes

#define IO_NORM "\033[0m"

#define IO_BLACK "\033[30m"

#define IO_RED "\033[31m"

#define IO_GREEN "\033[32m"

#define IO_YELLOW "\033[33m"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// String Matching

bool stringMatch(const char* const match, const char* const strings[], int len);

int stringMatchAt(const char* const match, const char* const strings[], int len);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flags Struct

struct Flags {
	int bits = 0;
	
	Flags();
	Flags(int bitsIn);

	// TODO: Implement these
	bool hasFlag(int flag);
	void setFlag(int flag);
	void unsetFlag(int flag);
	void toggleFlag(int flag);
};