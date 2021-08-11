#pragma once

#include <iostream>
#include <fstream>                                                                                                                                                                                                                                                                                       

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define ARR_LEN(arr) \
	sizeof(arr) / sizeof(arr[0])

#define TO_CH_PT(x) \
	reinterpret_cast<char*>(&x)

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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Number Parsing
typedef unsigned int uint;
// int is the return (0 if normal, non-0 if error)
int parseUInt(const char* str, uint& out) {
	uint base = 10;

	int strlen = 0;
	while (str[strlen] != '\0') strlen++;

	if (strlen > 2 && str[0] == '0') {
		char t = str[1];
		if (t < '0' || t > '9') {
			str += 2;
			strlen -= 2;
			switch (t) {
				case 'x':
					base = 16;
					break;

				case 'b':
					base = 2;
					break;

				case 'd':break;

				default:
					return 1;
			}
		}
	}

	uint place = 1;
	out = 0;
	char c;
	str += strlen - 1;
	while (strlen > 0) {
		c = *str;

		if ('0' <= c && c <= '9') {
			if (c - '0' >= base) return 2;
			out += place * (c - '0');
		} else if ('A' <= c && c <= 'Z') {
			if (c - 'A' + 10 >= base) return 2;
			out += place * (c - 'A' + 10);
		} else if ('a' <= c && c <= 'z') {
			if (c - 'a' + 10 >= base) return 2;
			out += place * (c - 'a' + 10);
		} else {
			return 3;
		}

		place *= base;

		strlen--;
		str--;
	}

	return 0;
}


