#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <string>
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
		std::cout << "Could not open file: " << str << "\n\n\n"; \
		return 1; \
	}

int assemble(const char* asmPath, const char* exePath, vm::AssemblyOptions options, bool debugMode) {
	std::fstream asm_;
	TRY_OPEN_FILE(asm_, asmPath, std::ios::in);

	std::fstream exe;
	TRY_OPEN_FILE(exe, exePath, std::ios::out | std::ios::binary | std::ios::trunc);

	try {
		if (debugMode) {
			vm::Assemble(asm_, exe, options, std::cout);
		} else {
			vm::Assemble(asm_, exe, options);
		}
	} catch (vm::AssemblyError e) {
		std::cout << "#### Program Assembled With Error\n     " << e.what() << "\n\n\n";
		return 1;
	}

	std::cout << "\n\n\n";

	return 0;
}

int exec(const char* exePath, vm::ExecOptions options, bool debugMode) {
	std::fstream exe;
	TRY_OPEN_FILE(exe, exePath, std::ios::in | std::ios::binary);

	try {
		if (debugMode) {
			vm::Exec(exe, std::cout, options, std::cout);
		} else {
			vm::Exec(exe, std::cout, options);
		}
	} catch (vm::ExecError e) {
		std::cout << "#### Program Executed With Error\n     " << e.what() << "\n\n\n";
		return 1;
	}

	std::cout << "\n\n\n";

	return 0;
}

int main(int argc, const char* args[]) {

	vm::AssemblyOptions asmOptions;
	vm::ExecOptions exeOptions;
	bool debugMode = false;

	std::string arg;
	std::string possArgs[] = {
		"-assemble",
		"-exec",
		"-debug",
		"-nodebug",
		"-profile"
	};
	constexpr int numPossArgs = sizeof(possArgs) / sizeof(std::string);

	for (int j, i = 0; i < argc; i++) {
		arg = args[i];
		for (j = 0; j < numPossArgs; j++) {
			if (arg == possArgs[j]) {
				switch (j) {
					case 0: // assemble
						if (argc - i < 3) {
							std::cout << "Not enough arguments for assemble\n\n\n";
							return 1;
						}
						if (assemble(args[i + 1], args[i + 2], asmOptions, debugMode)) {
							return 1;
						}
						break;

					case 1: // exec
						if (argc - i < 2) {
							std::cout << "Not enough arguments for exec\n\n\n";
							return 1;
						}
						if (exec(args[i + 1], exeOptions, debugMode)) {
							return 1;
						}
						break;

					case 2: // debug
						debugMode = true;
						break;

					case 3: // nodebug
						debugMode = false;
						break;

					case 4: // profile
						exeOptions.flags |= vm::ExecOptions::PROFILE;
						break;
				}
				break;
			}
		}
	}

	std::cout << "\n\n\n";
	return 0;
}