#pragma once
#include "vm.h"

#define EXE_THROW(x) \
	throw vm::ExecError(vm::ExecError::x)

#define REINT_CAST(type, s) \
	(*reinterpret_cast<type*>(&(s)))