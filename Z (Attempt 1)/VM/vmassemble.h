// Assemble hand-written bytecode(.azm) into binary bytecode(.eze)
#include "vm.h"
#include "vmassembledefs.h"

void vm::Assemble(std::iostream& azm,
				  std::ostream& eze,
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
	literal_t lit = 0;
	int argc = 0;

	bool end = false;
	bool isComment = false;
	int pCount = 0;
	azm.seekg(0);
	eze.seekp(0);

#ifdef VM_DEBUG
	debug << "Byteode assembly debug:";
#endif
	while (!end) {
		azm.get(c);
		end = azm.eof();

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
					THROW(UNBALANCED_PARENS);
				}
				continue;
			}
		}

		if (pCount < 0) {
			THROW(UNBALANCED_PARENS);
		} else if (pCount > 0 || isComment) {
			continue;
		} 
		
		if (c == ' ' || c == ',' || c == '\n' || c == '\t' || end) {
			if (len == 0)
				continue;
			str[len++] = '\0';

			if (opcode == NOP || args[opcode][argc] == ARG_NONE) {
			#ifdef VM_DEBUG
				debug << '\n';
			#endif
				parseOpcode(opcode, str);
				eze.write(TO_CHAR(opcode), sizeof(opcode_t));
				argc = 0;

			#ifdef VM_DEBUG
				debug << vm::Opcode::strings[opcode] << ' ';
			#endif
			} else {
				switch (args[opcode][argc]) {
					case ARG_NONE:
						argc = MAX_ARGS;
						break;
					case ARG_REGISTER:
						parseRegister(reg, str);
						eze.write(TO_CHAR(reg), sizeof(register_t));
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;
					case ARG_HEX_LITERAL:
						parseLiteral(lit, str);
						eze.write(TO_CHAR(lit), sizeof(literal_t));
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;
				}
				if (argc++ >= MAX_ARGS) {
					opcode = NOP;
				}
			}
			len = 0;
		} else {
			if (len >= STR_SIZE - 2) {
				THROW(MAX_STR_SIZE_REACHED);
			}

			str[len++] = c;
		}
	}
}