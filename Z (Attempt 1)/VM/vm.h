// Includes the "vmassemble.h" and "vmexec.h" files in both debug and normal modes
#pragma once
#include "vmsetup.h"

#define VM_DEBUG
#include "vmassemble.h"
#include "vmexec.h"
#undef VM_DEBUG
#include "vmassemble.h"
#include "vmexec.h"