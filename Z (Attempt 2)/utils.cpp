#include "utils.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// String Matching

bool stringMatch(const char* const match, const char* const strings[], int len) {
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

int stringMatchAt(const char* const match, const char* const strings[], int len) {
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

Flags::Flags() {}
Flags::Flags(int bitsIn) : bits(bitsIn) {}