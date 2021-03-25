#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include "VM/vm.h"

/*#define VM_DEBUG
#include "VM/vmassemble.h"
#include "VM/vmexec.h"
#undef VM_DEBUG
#include "VM/vmassemble.h"
#include "VM/vmexec.h"*/

#define TRY_OPEN_FILE(var, str, mode) \
	var.open(str, mode); \
	if (!var.is_open()) { \
		std::cout << ("Could not open file" str "\n\n"); \
		return 1; \
	}

int main(int argc, const char* args[]) {

	std::fstream azm;
	TRY_OPEN_FILE(azm, "..\\Examples\\example1.azm", std::ios::in);

	std::fstream eze;
	TRY_OPEN_FILE(eze, "..\\Examples\\example1.eze", std::ios::out | std::ios::in | std::ios::binary);

	try {
		vm::Assemble(azm, eze, vm::AssemblyOptions(), std::cout);
	} catch (vm::AssemblyError e) {
		std::cout << e.what() << "\n\n\n";
		return 1;
	}

	std::cout << "\n\n";

	try {
		vm::Exec(eze, vm::ExecOptions(), std::cout);
	} catch (vm::ExecError e) {
		std::cout << e.what() << "\n\n\n";
		return 1;
	}

	std::cout << "\n\n\n";
	return 0;
}