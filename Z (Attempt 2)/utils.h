#pragma once

#include <iostream>
#include <fstream>                                                                                                                                                                                                                                                                                       

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define ARR_LEN(arr) \
	sizeof(arr) / sizeof(arr[0])

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ANSI codes (and other io utils)

#define IO_END "\n\n\n"

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
	int bits;
	
	Flags();
	Flags(int bitsIn);

	// TODO: Implement these
	bool hasFlags(int flags);
	void setFlags(int flags);
	void unsetFlags(int flags);
	void toggleFlags(int flags);
};