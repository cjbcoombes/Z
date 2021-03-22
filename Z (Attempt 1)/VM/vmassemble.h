// Assemble hand-written bytecode(.azm) into binary bytecode(.eze)
#include "vm.h"

#ifndef VM_ASSEMBLE_INCLUDED
void parseOpcode(vm::types::opcode_t& opcode, char* const& str) {
	const char* curr;
	int j;
	for (opcode = 0; opcode < vm::Opcode::count; opcode++) {
		curr = vm::Opcode::strings[opcode];
		j = 0;
		while (true) {
			if (str[j] != curr[j])
				goto next;
			
			if (str[j] == '\0')
				return;
		}
	next:;
	}

	throw vm::AssemblyError(vm::AssemblyError::UNKNOWN_OPCODE);
}
#endif

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

	bool end = false;
	azm.seekg(0);
	while (!end) {
		azm.get(c);
		end = azm.eof();

		if (c == ' ' || c == '\n' || c == '\t' || end) {
			if (len == 0)
				continue;

			str[len++] = '\0';

			parseOpcode(opcode, str);

		#ifdef VM_DEBUG
			debug << vm::Opcode::strings[opcode] << ' ';
		#endif

		} else {
			if (len >= STR_SIZE - 2) {
				throw vm::AssemblyError(vm::AssemblyError::MAX_STR_SIZE_REACHED);
			}
			str[len++] = c;
		}
	}
}

#define VM_ASSEMBLE_INCLUDED