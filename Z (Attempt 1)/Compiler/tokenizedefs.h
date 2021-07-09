#pragma once
#include "compile.h"

bool matchCharIn(char c, char* const& list) {
	int i = 0;
	while (true) {
		if (list[i] == c) return true;
		if (list[i++] == '\0') return false;
	}
}