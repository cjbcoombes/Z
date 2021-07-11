#include "main.h"

using std::cout;

int assemble(const char* const assemblyPath, const char* const outputPath, Flags assemblyFlags) {
	cout << "Attempting to assemble file \"" << assemblyPath << "\" into output file \"" << outputPath << "\"\n";

	std::fstream assemblyFile, outputFile;

	assemblyFile.open(assemblyPath, std::ios::in);
	if (!assemblyFile.is_open()) {
		cout << IO_ERR "Could not open file \"" << assemblyPath << "\"" IO_NORM IO_END;
		return 1;
	}

	outputFile.open(outputPath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!outputFile.is_open()) {
		cout << IO_ERR "Could not open file \"" << outputPath << "\"" IO_NORM IO_END;
		return 1;
	}

	return 0;
}

int exec(const char* const path, Flags execFlags) {
	cout << "Attempting to execute file \"" << path << "\"\n";
	
	std::fstream file;
	file.open(path, std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		cout << IO_ERR "Could not open file \"" << path << "\"" IO_NORM IO_END;
		return 1;
	}

	return 0;
}

int main(int argc, const char* args[]) {

	cout << IO_NORM;

	const char* const commands[] = {
		"-assemble",
		"-exec",
		"-asmandexec",
		"-debug",
		"-nodebug",
		"-profile",
		"-noprofile"
	};

	Flags assemblyFlags;
	Flags execFlags;

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
					if (assemble(args[i+1], args[i+2], assemblyFlags)) return 1;
					i += 2;
				}
				break;

			case 1: // -exec
				if (argc - i < 2) {
					cout << IO_ERR "Not enough arguments for execution" IO_NORM IO_END;
					return 1;
				} else {
					if (exec(args[i + 1], execFlags)) return 1;
					i++;
				}
				break;

			case 2: // -asmandexec
				if (argc - i < 3) {
					cout << IO_ERR "Not enough arguments for assembler and execution" IO_NORM IO_END;
					return 1;
				} else {
					if (assemble(args[i + 1], args[i + 2], assemblyFlags)) return 1;
					if (exec(args[i + 2], execFlags)) return 1;
					i += 2;
				}
				break;

			case 3: // -debug
				assemblyFlags.setFlags(vm::FLAG_DEBUG);
				execFlags.setFlags(vm::FLAG_DEBUG);
				break;

			case 4: // -nodebug
				assemblyFlags.unsetFlags(vm::FLAG_DEBUG);
				execFlags.unsetFlags(vm::FLAG_DEBUG);
				break;

			case 5: // -profile
				execFlags.setFlags(vm::FLAG_PROFILE);
				break;

			case 6: // -noprofile
				execFlags.unsetFlags(vm::FLAG_PROFILE);
				break;
		}
	}

}
