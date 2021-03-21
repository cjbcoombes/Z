// Includes the "vmassemble.h" and "vmexec.h" files in both debug and normal modes
#include "vm.h"

#define VM_DEBUG
#include "vmassemble.h"
#include "vmexec.h"
#undef VM_DEBUG
#include "vmassemble.h"
#include "vmexec.h"