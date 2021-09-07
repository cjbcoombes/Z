#include "utils.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// String Matching

bool stringMatch(const char* const& match, const char* const strings[], const int& len) {
	int i = 0;
	int j = 0;
	
	for (; i < len; i++) {
		j = 0;
		while (true) {
			if (strings[i][j] != match[j]) break;
			if (match[j] == '\0') return true;
			j++;
		}
	}

	return false;
}

int stringMatchAt(const char* const& match, const char* const strings[], const int& len) {
	int i = 0;
	int j = 0;

	for (; i < len; i++) {
		j = 0;
		while (true) {
			if (strings[i][j] != match[j]) break;
			if (match[j] == '\0') return i;
			j++;
		}
	}

	return -1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flags Struct

Flags::Flags() : bits(0) {}
Flags::Flags(const int& bitsIn) : bits(bitsIn) {}

bool Flags::hasFlags(const int& flags) {
	return !(bits ^ flags);
}

void Flags::setFlags(const int& flags) {
	bits |= flags;
}

void Flags::unsetFlags(const int& flags) {
	bits &= ~flags;
}

void Flags::toggleFlags(const int& flags) {
	bits ^= flags;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Number Parsing

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
