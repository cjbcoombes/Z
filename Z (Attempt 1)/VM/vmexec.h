// Execute binary bytecode(.eze)
#include "vmexecdefs.h"

void vm::Exec(std::iostream& eze,
			  ExecOptions options
		  #ifdef VM_DEBUG
			  , std::ostream& debug
		  #endif
) {
	using namespace vm::types;
	using namespace vm::Opcode;
	using vm::FileBuffer;
	using vm::Stack;

	FileBuffer buffer(eze);

	Stack stack;
	literal_t regs[vm::Register::GP_REGISTER_COUNT + vm::Register::GP_REGISTER_OFFSET];
	regs[0] = REINT_CAST(literal_t, stack.bottom);// BP
	regs[1] = 0;// CP

	opcode_t opcode = NOP;
	register_t reg1 = 0;
	register_t reg2 = 0;
	register_t reg3 = 0;
	literal_t lit = 0;
	address_t addr = 0;

#ifdef VM_DEBUG
	std::cout << STRM_DEFAULT << "Bytecode execution debug:\n" << STRM_HEX;
#endif
	while (!buffer.atEnd()) {
		buffer.read<opcode_t>(&opcode);
		// If sizeof(opcode_t) > 1 then buffer.readCheck<opcode_t>(&test);

	/*#ifdef VM_DEBUG
		debug << std::setw(2) << static_cast<uint16_t>(test) << ' ';
	#endif*/

		switch (opcode) {
			case NOP:
				break;

			case HALT:
				// Mem cleanup?
				goto end;
				break;
			
			case MOV:
				buffer.checkNum(sizeof(register_t) + sizeof(register_t));
				// Check register bounds?
				buffer.read<register_t>(&reg1);
				buffer.read<register_t>(&reg2);
				regs[reg1] = regs[reg2];
				break;

			case MOVL:
				buffer.checkNum(sizeof(register_t) + sizeof(literal_t));
				// Check register bounds?
				buffer.read<register_t>(&reg1);
				buffer.read<literal_t>(&lit);
				regs[reg1] = lit;
				break;
		}
	}

end:;
}