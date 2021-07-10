#pragma once

#include "utils.h"
#include "VM/vm.h"

int assemble(const char* const assemblyFile, const char* const outputFile, Flags assemblyFlags);

int exec(const char* const path, Flags execFlags);