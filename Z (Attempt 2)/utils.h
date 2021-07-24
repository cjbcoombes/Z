#pragma once

#include <iostream>
#include <fstream>                                                                                                                                                                                                                                                                                       

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define ARR_LEN(arr) \
	sizeof(arr) / sizeof(arr[0])

#define TO_CH_PT(x) \
	reinterpret_cast<char*>(x)

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ANSI codes (and other io utils)

#define IO_END "\n\n\n"

#define IO_NORM "\033[0m"
#define IO_BLACK "\033[30m"
#define IO_RED "\033[31m"
#define IO_GREEN "\033[32m"
#define IO_YELLOW "\033[33m"
#define IO_BLUE "\033[34m"
#define IO_MAGENTA "\033[35m"
#define IO_CYAN "\033[36m"
#define IO_GRAY "\033[90m"

#define IO_ERR IO_RED "[ERROR] "
#define IO_WARN IO_YELLOW "[WARNING] "
#define IO_OK IO_GREEN
#define IO_DEBUG IO_CYAN "[DEBUG] "

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// String Matching

bool stringMatch(const char* const& match, const char* const strings[], const int& len);

int stringMatchAt(const char* const& match, const char* const strings[], const int& len);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flags Struct

struct Flags {
	int bits;
	
	Flags();
	Flags(const int& bitsIn);

	// TODO: Implement these
	bool hasFlags(const int& flags);
	void setFlags(const int& flags);
	void unsetFlags(const int& flags);
	void toggleFlags(const int& flags);
};

