#include "vm.h"

#include <unordered_map>

using vm::assembler::AssemblerException;
using std::cout;

#define ASM_DEBUG(thing) if (isDebug) stream << IO_DEBUG << thing << IO_NORM "\n"

int vm::assembler::assemble(const char* const& assemblyPath, const char* const& outputPath, Flags& assemblyFlags) {
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
		return vm::assembler::assemble_(assemblyFile, outputFile, assemblyFlags, std::cout);
	} catch (AssemblerException& e) {
		cout << IO_ERR "Error during assembly at LINE " << e.line << ", COLUMN " << e.column << ": " << e.what() << IO_NORM IO_END;
	} catch (std::exception& e) {
		cout << IO_ERR "An unknown error ocurred during assembly. This error is most likely an issue with the c++ assembler code, not your code. Sorry. The provided error message is as follows:\n" << e.what() << IO_NORM IO_END;
	}

	return 1;
}

int vm::assembler::assemble_(std::iostream& assemblyFile, std::iostream& outputFile, Flags& assemblyFlags, std::ostream& stream) {
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

	//std::unordered_map<std::string, 

	int carg = 0;
	opcode_t opcode = NOP;

	word_t word = 0;
	byte_t byte = 0;
	short_t short_ = 0;

	//

	while (!end) {
		assemblyFile.get(c);
		std::cout << c << '\n';
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
			if (strlen == 0) continue;
			str[strlen++] = '\0';

			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

			if (opcode == NOP) {
				if (str[0] == '@') {
					// Process label
				}

				opcode = stringMatchAt(str, vm::opcode::strings, vm::opcode::count);
				carg = 0;
				ASM_DEBUG("Opcode: " << static_cast<int>(opcode));

			} else {
				switch (args[opcode][carg]) {
					case 0: // ARG_NONE
						carg = MAX_ARGS;
						break;

					case 1: // ARG_REG
						break;

					case 2: // ARG_WORD
						// TODO : Parse labels
						word = parseNumber<word_t, AssemblerException::INVALID_WORD_PARSE>(str, strlen, line, column);
						outputFile.write(TO_CH_PT(word), sizeof(word_t));
						break;

					case 3: // ARG_BYTE
						byte = parseNumber<byte_t, AssemblerException::INVALID_BYTE_PARSE>(str, strlen, line, column);
						outputFile.write(TO_CH_PT(byte), sizeof(byte_t));
						break;

					case 4: // ARG_SHORT
						short_ = parseNumber<short_t, AssemblerException::INVALID_SHORT_PARSE>(str, strlen, line, column);
						outputFile.write(TO_CH_PT(short_), sizeof(short_t));
						break;
				}

				if (carg >= MAX_ARGS) {
					opcode = NOP;
				}
			}

			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Parsing

vm::types::reg_t vm::assembler::parseRegister(char* const& str, const int& strlen, const int& line, const int& column) {
	if (strlen < 2) throw AssemblerException(AssemblerException::INVALID_REG_PARSE, line, column);

	if (str[0] == 'B' && str[1] == 'P') {
		return register_::BP;
	} else if (str[0] == 'G' && str[1] == 'P') {
		return register_::GP;
	} else if (str[0] == 'R') {
		types::reg_t out = parseNumber<types::reg_t, AssemblerException::INVALID_REG_PARSE>(str + 1, strlen - 1, line, column);

		if (out >= register_::NUM_GEN_REGISTERS || out < 0) {
			throw AssemblerException(AssemblerException::INVALID_REG_PARSE, line, column);
		}

		return out;
	}

	return 0;
}

template<typename T, vm::assembler::AssemblerException::ErrorType eType>
T vm::assembler::parseNumber(char* const& str, int strlen, const int& line, const int& column) {
	char t = str[strlen - 1];
	T base = 10;

	switch (t) {
		case 'x':
			base = 16;
			break;

		case 'b':
			base = 2;
			break;

		case 'd':break;

		default:
			if ('0' <= t && t <= '9') {
				strlen++;
				break;
			}
			throw AssemblerException(eType, line, column);
	}
	// TODO: Negative #s
	T place = 1, out = 0;
	for (int i = 0; i < strlen - 1; i++) {
		char c = str[strlen - 2 - i];

		if ('0' <= c && c <= '9') {
			if (c - '0' >= base) throw AssemblerException(eType, line, column);
			out += place * (c - '0');
		} else if ('A' <= c && c <= 'Z') {
			if (c - 'A' + 10 >= base) throw AssemblerException(eType, line, column);
			out += place * (c - 'A' + 10);
		} else if ('a' <= c && c <= 'z') {
			if (c - 'a' + 10 >= base) throw AssemblerException(eType, line, column);
			out += place * (c - 'a' + 10);
		}

		place *= base;
	}

	return out;
}