// Includes the "tokenize.h" file in both debug and normal modes
#include "compile.h"

#define COMP_DEBUG
#include "tokenize.h"
#undef COMP_DEBUG
#include "tokenize.h"