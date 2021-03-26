// Execute binary bytecode(.exe)
#include "vmexecdefs.h"

void vm::Exec(std::iostream& exe,
			  std::ostream& output,
			  ExecOptions options
		  #ifdef VM_DEBUG
			  , std::ostream& debug
		  #endif
) {
	using namespace vm::types;
	using namespace vm::Opcode;
	using vm::FileBuffer;
	using vm::Stack;

	FileBuffer buffer(exe);

	Stack stack;
	Value regs[vm::Register::GP_REGISTER_COUNT + vm::Register::GP_REGISTER_OFFSET];
	regs[0].addr = reinterpret_cast<address_t>(stack.bottom);// BP
	regs[1].i = 0;// CP

	opcode_t opcode = NOP;
	register_t reg1 = 0;
	register_t reg2 = 0;
	register_t reg3 = 0;
	word_t word = 0;
	byte_t byte = 0;
	address_t addr = 0;

#ifdef VM_DEBUG
	debug << STRM_DEFAULT << "Bytecode execution debug:\n";
#endif
	while (!buffer.atEnd()) {
		buffer.read<opcode_t>(&opcode);
		// If sizeof(opcode_t) > 1 then buffer.readCheck<opcode_t>(&test);

	#ifdef VM_DEBUG
		// debug << '[' << std::setw(2) << static_cast<uint16_t>(opcode) << "] ";
	#endif

		switch (opcode) {
			case NOP:
				break;

			case HALT:
				// Mem cleanup?
				goto end;
				break;

			case RPRNT:
				buffer.readCheck<register_t>(&reg1);
				output << STRM_HEX << "0x" << std::setw(sizeof(word_t) * 2) << regs[reg1].word << STRM_DEFAULT;
				break;

			case LNPRNT:
				output << '\n';
				break;

			case MOV:
				buffer.checkNum(sizeof(register_t) * 2);
				// Check register bounds?
				buffer.read<register_t>(&reg1);
				buffer.read<register_t>(&reg2);
				regs[reg1] = regs[reg2];
				break;

			case MOVW:
				buffer.checkNum(sizeof(register_t) + sizeof(word_t));
				// Check register bounds?
				buffer.read<register_t>(&reg1);
				buffer.read<word_t>(&word);
				regs[reg1].word = word;
				break;

			case MOVB:
				buffer.checkNum(sizeof(register_t) + sizeof(byte_t));
				// Check register bounds?
				buffer.read<register_t>(&reg1);
				buffer.read<byte_t>(&byte);
				regs[reg1].byte = byte;
				break;

			case LOADW:
				buffer.checkNum(sizeof(register_t) * 2 + sizeof(address_t));
				buffer.read<register_t>(&reg1);
				buffer.read<register_t>(&reg2);
				buffer.read<address_t>(&addr);
			#pragma warning( push )
			#pragma warning( disable: 6011 )// Dereference potentially-null pointer
				regs[reg1].word = *(reinterpret_cast<word_t*>(regs[reg2].addr + addr));
			#pragma warning( pop )
				break;

			case STOREW:
				buffer.checkNum(sizeof(register_t) * 2 + sizeof(address_t));
				buffer.read<register_t>(&reg1);
				buffer.read<address_t>(&addr);
				buffer.read<register_t>(&reg2);
			#pragma warning( push )
			#pragma warning( disable: 6011 )// Dereference potentially-null pointer
				(*(reinterpret_cast<word_t*>(regs[reg1].addr + addr))) = regs[reg2].word;
			#pragma warning( pop )
				break;

			case IADD:
				buffer.checkNum(sizeof(register_t) * 3);
				// Check register bounds?
				buffer.read<register_t>(&reg1);
				buffer.read<register_t>(&reg2);
				buffer.read<register_t>(&reg3);
				regs[reg1].i = regs[reg2].i + regs[reg3].i;
				break;

			default:
				EXE_THROW(UNKNOWN_OPCODE);
		}
	}

end:;
}