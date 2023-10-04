#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>                                                                                                                                                                                                                                                                                       

#include "register.h"
#include "opcode.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define ARR_LEN(arr) \
	sizeof(arr) / sizeof(arr[0])

#define TO_CH_PT(x) \
	reinterpret_cast<char*>(&(x))

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
#define IO_MAIN IO_GREEN "[MAIN] "
#define IO_DEBUG IO_CYAN "[DEBUG] "

#define IO_HEX std::hex
#define IO_DEC std::dec

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
int parseUInt(const char* str, uint& out);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Types
namespace types {
	// 32-bit word: addresses, integers
	typedef int32_t word_t;
	// 8-bit byte: register IDs, opcode IDs, chars
	typedef int8_t byte_t;

	// Opcode ID: byte
	typedef uint8_t opcode_t;
	// Register ID: byte
	typedef uint8_t reg_t;

	// Integer: word
	typedef int32_t int_t;
	typedef float float_t;
	// Char: byte
	typedef int8_t char_t;
	// Bool: byte
	typedef int8_t bool_t;

	union WordVal {
		word_t word;

		int_t int_;
		float_t float_;
	};

	union ByteVal {
		byte_t byte;

		char_t char_;
		bool_t bool_;
	};

	static_assert(sizeof(std::intptr_t) == sizeof(word_t), "No workaround for non-word-size (32-bit) pointers");
	static_assert(sizeof(float) == sizeof(word_t), "No workaround for non-word-size (32-bit) floats");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flags
constexpr int FLAG_DEBUG = 1;
constexpr int FLAG_PROFILE = 2;

