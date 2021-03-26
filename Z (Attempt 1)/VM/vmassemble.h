// Assemble hand-written bytecode(.azm) into binary bytecode(.eze)
#include "vmassembledefs.h"

void vm::Assemble(std::iostream& asm_,
				  std::ostream& exe,
				  AssemblyOptions options
			  #ifdef VM_DEBUG
				  , std::ostream& debug
			  #endif
) {
	using namespace vm::types;
	using namespace vm::Opcode;

	constexpr int STR_SIZE = 256;
	char str[STR_SIZE];
	int len = 0;
	char c;

	opcode_t opcode = NOP;
	register_t reg = 0;
	word_t word = 0;
	address_t addr = 0;
	byte_t byte = 0;
	int argc = 0;

	bool end = false;
	bool isComment = false;
	int pCount = 0;
	asm_.seekg(0);
	exe.seekp(0);

#ifdef VM_DEBUG
	debug << STRM_DEFAULT << "Bytecode assembly debug:";
#endif
	while (!end) {
		asm_.get(c);
		end = asm_.eof();

		if (!end) {
			if (c == ';') {
				isComment = true;
			} else if (c == '\n') {
				if (isComment) {
					isComment = false;
					continue;
				}
			} if (c == '(') {
				pCount++;
			} else if (c == ')') {
				if (!(pCount--)) {
					ASM_THROW(UNBALANCED_PARENS);
				}
				continue;
			}
		}

		if (pCount < 0) {
			ASM_THROW(UNBALANCED_PARENS);
		} else if (pCount > 0 || isComment) {
			continue;
		} 
		
		if (c == ' ' || c == ',' || c == '\n' || c == '\t' || end) {
			if (len == 0)
				continue;
			str[len++] = '\0';

			if (opcode == NOP || args[opcode][argc] == ArgType::ARG_NONE) {
			#ifdef VM_DEBUG
				debug << '\n';
			#endif
				parseOpcode(opcode, str);
				exe.write(TO_CHAR(opcode), sizeof(opcode_t));
				argc = 0;

			#ifdef VM_DEBUG
				debug << vm::Opcode::strings[opcode] << ' ';
			#endif
			} else {
				switch (args[opcode][argc]) {
					case ArgType::ARG_NONE:
						argc = MAX_ARGS;
						break;

					case ArgType::ARG_REG:
						parseRegister(reg, str);
						exe.write(TO_CHAR(reg), sizeof(register_t));
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;

					case ArgType::ARG_WORD:
						parseWord(word, str);
						exe.write(TO_CHAR(word), sizeof(word_t));
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;

					case ArgType::ARG_BYTE:
						parseByte(byte, str);
						exe.write(TO_CHAR(byte), sizeof(byte_t));
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;

					// TODO: ARG_OFF
				}
				if (argc++ >= MAX_ARGS) {
					opcode = NOP;
				}
			}
			len = 0;
		} else {
			if (len >= STR_SIZE - 2) {
				ASM_THROW(MAX_STR_SIZE_REACHED);
			}

			str[len++] = c;
		}
	}

#ifdef VM_DEBUG
	debug << '\n';
#endif
}