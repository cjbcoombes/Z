// Assemble hand-written bytecode(.azm) into binary bytecode(.eze)
#include "vm.h"

void vm::Assemble(std::iostream azm,
				  std::ostream eze,
				  AssemblyOptions options
			  #ifdef VM_DEBUG
				  , std::ostream debug
			  #endif
) {
	char str[256];
	int len = 0;
	char c;
	bool end = false;

	while (!end) {
		azm.get(c);
		end = azm.eof();

		if (c == ' ' || c == '\n' || c == '\t' || end) {
			if (len == 0)
				continue;
		} else {
			if (len >= 255) {
				throw vm::AssemblyError(vm::AssemblyError::MAX_STR_SIZE_REACHED);
			}
			str[len++] = c;
		}
	}
}