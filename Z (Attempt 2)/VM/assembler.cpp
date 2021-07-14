#include "vm.h"

using vm::assembler::AssemblerException;
using std::cout;

int vm::assembler::assemble(const char* const assemblyPath, const char* const outputPath, Flags assemblyFlags) {
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

	try {
		return vm::assembler::assemble_(assemblyFile, outputFile, assemblyFlags);
	} catch (AssemblerException& e) {
		cout << IO_ERR "Error during assembly at LINE " << e.line << ", COLUMN " << e.column << ": " << e.what() << IO_NORM IO_END;
	} catch (std::exception& e) {
		cout << IO_ERR "An unknown error ocurred during assembly. This error is most likely an issue with the c++ assembler code, not your code. Sorry. The provided error message is as follows:\n" << e.what() << IO_NORM IO_END;
	}

	return 1;
}

int vm::assembler::assemble_(std::iostream& assemblyFile, std::iostream& outputFile, Flags assemblyFlags) {
	using namespace vm::opcode;
	using namespace vm::types;
	
	const bool isDebug = assemblyFlags.hasFlags(vm::FLAG_DEBUG);

	assemblyFile.seekg(0);
	outputFile.seekp(0);

	char str[vm::assembler::MAX_STR_SIZE];
	int strlen = 0;
	char c;
	int line = 0;
	int column = 0;

	bool end = false;
	bool isComment = false;
	bool isParen = false;

	//
	
	opcode_t opcode = NOP;
	

	//

	while (!end) {
		assemblyFile.get(c);
		if (c == '\n') {
			line++;
			column = 0;
		} else {
			column++;
		}
		if (assemblyFile.eof()) end = true;

		if (isComment) {
			if (c == '\n') isComment = false;
			continue;
		}

		if (isParen) {
			if (c == ')') isParen = false;
			continue;
		}

		if (c == ';') {
			isComment = true;
			continue;
		}

		if (c == '(') {
			isParen = true;
			continue;
		}

		if (c == ' ' || c == ',' || c == '\n' || c == '\t') {
			// Process the string
			strlen = 0;
			continue;
		}

		if (strlen >= MAX_STR_SIZE) {
			throw AssemblerException(AssemblerException::STRING_TOO_LONG, line, column);
		}
		str[strlen++] = c;
	}

	return 0;
}