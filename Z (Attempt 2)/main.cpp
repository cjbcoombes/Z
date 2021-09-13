#include "main.h"

using std::cout;

int main(int argc, const char* args[]) {

	cout << IO_NORM;

	const char* const commands[] = {
		"-assemble",
		"-exec",
		"-asmandexec",
		"-debug",
		"-nodebug",
		"-profile",
		"-noprofile",
		"-stacksize",
		"-compile"
	};

	vm::assembler::AssemblerSettings assemblerSettings;
	vm::executor::ExecutorSettings executorSettings;
	compiler::CompilerSettings compilerSettings;

	// Dummy Variables
	uint uInt = 0;

	for (int i = 1; i < argc; i++) {
		switch (stringMatchAt(args[i], commands, ARR_LEN(commands))) {
			case -1:
				cout << IO_WARN "Unknown console command: " << args[i] << IO_NORM "\n";
				break;

			case 0: // -assemble
				if (argc - i < 3) {
					cout << IO_ERR "Not enough arguments for assembler" IO_NORM IO_END;
					return 1;
				} else {
					if (vm::assembler::assemble(args[i + 1], args[i + 2], assemblerSettings)) return 1;
					i += 2;
				}
				break;

			case 1: // -exec
				if (argc - i < 2) {
					cout << IO_ERR "Not enough arguments for execution" IO_NORM IO_END;
					return 1;
				} else {
					if (vm::executor::exec(args[i + 1], executorSettings)) return 1;
					i++;
				}
				break;

			case 2: // -asmandexec
				if (argc - i < 3) {
					cout << IO_ERR "Not enough arguments for assembler and execution" IO_NORM IO_END;
					return 1;
				} else {
					if (vm::assembler::assemble(args[i + 1], args[i + 2], assemblerSettings)) return 1;
					if (vm::executor::exec(args[i + 2], executorSettings)) return 1;
					i += 2;
				}
				break;

			case 3: // -debug
				assemblerSettings.flags.setFlags(FLAG_DEBUG);
				executorSettings.flags.setFlags(FLAG_DEBUG);
				break;

			case 4: // -nodebug
				assemblerSettings.flags.unsetFlags(FLAG_DEBUG);
				executorSettings.flags.unsetFlags(FLAG_DEBUG);
				break;

			case 5: // -profile
				executorSettings.flags.setFlags(FLAG_PROFILE);
				break;

			case 6: // -noprofile
				executorSettings.flags.unsetFlags(FLAG_PROFILE);
				break;

			case 7: // -stacksize
				if (argc - i < 2) {
					cout << IO_ERR "Not enough arguments for setting stack size" IO_NORM IO_END;
					return 1;
				} else if (parseUInt(args[i + 1], uInt)) {
					cout << IO_ERR "Invalid stack size" IO_NORM IO_END;
					return 1;
				} else {
					executorSettings.stackSize = uInt;
				}
				break;

			case 8: // -compile
				if (argc - i < 3) {
					cout << IO_ERR "Not enough arguments for compiler" IO_NORM IO_END;
					return 1;
				} else {
					if (compiler::compile(args[i + 1], args[i + 2], compilerSettings)) return 1;
					i += 2;
				}
				break;
		}
	}

}
