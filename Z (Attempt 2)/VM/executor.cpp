#include "vm.h"
#include <limits>

using std::cout;

#define AS_WORD(x) \
	reinterpret_cast<vm::types::word_t*>(x)

int vm::executor::exec(const char* const& path, ExecutorSettings& execSettings) {
	cout << "Attempting to execute file \"" << path << "\"\n";

	std::fstream file;
	file.open(path, std::ios::in | std::ios::binary);

	try {
		return vm::executor::exec_(file, execSettings, std::cout);
	} catch (ExecutorException& e) {
		cout << IO_ERR "Error during execution at BYTE" << e.loc << " : " << e.what() << IO_NORM IO_END;
	} catch (std::exception& e) {
		cout << IO_ERR "An unknown error ocurred during execution. This error is most likely an issue with the c++ executor code, not your code. Sorry. The provided error message is as follows:\n" << e.what() << IO_NORM IO_END;
	}

	return 1;
}

int vm::executor::exec_(std::iostream& file, ExecutorSettings& execSettings, std::ostream& stream) {
	using namespace types;
	using namespace opcode;

	Program program(file);
	Stack stack(execSettings.stackSize);
	Value reg[register_::R0 + register_::NUM_GEN_REGISTERS];
	reg[register_::PP].word = *reinterpret_cast<word_t*>(&program.start);
	reg[register_::BP].word = *reinterpret_cast<word_t*>(&stack.start);

	program.goto_(format::FIRST_INSTR_ADDR_LOCATION);
	program.goto_(*AS_WORD(program.ip));

	// Dummy values
	opcode_t opcode = 0;
	reg_t rid1 = 0;
	reg_t rid2 = 0;
	reg_t rid3 = 0;
	word_t word = 0;
	byte_t byte = 0;
	short_t short_ = 0;
	int_t int_ = 0;
	char_t char_ = 0;

	for (; program.ip < program.end; program.ip++) {
		opcode = *program.ip;
		switch (opcode) {
			case NOP:
				break;

			case HALT:
				// TODO: automatic memory cleanup
				return 0;

			case R_PRNT_W:
				program.read<word_t>(&word);
				stream << "0x" << IO_HEX << word << IO_DEC;
				break;

			case LN_PRNT:
				stream << '\n';
				break;

			case MOV:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				reg[rid1] = reg[rid2];
				break;

			case MOV_W: 
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				reg[rid1].word = word;
				break;

			case MOV_B:
				program.read<reg_t>(&rid1);
				program.read<byte_t>(&byte);
				reg[rid1].byte = byte;
				break;

			case MOV_S:
				program.read<reg_t>(&rid1);
				program.read<short_t>(&short_);
				reg[rid1].short_ = short_;
				break;

			case LOAD_W: 
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<word_t>(&word);
				reg[rid1].word = *reinterpret_cast<word_t*>(reg[rid2].word + word);
				break;

			case STORE_W: 
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				program.read<reg_t>(&rid2);
				*reinterpret_cast<word_t*>(reg[rid1].word + word) = reg[rid2].word;
				break;

			case LOAD_B:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<word_t>(&word);
				reg[rid1].byte = *reinterpret_cast<byte_t*>(reg[rid2].word + word);
				break;

			case STORE_B:
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				program.read<reg_t>(&rid2);
				*reinterpret_cast<byte_t*>(reg[rid1].word + word) = reg[rid2].byte;
				break;

			case LOAD_S:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<word_t>(&word);
				reg[rid1].short_ = *reinterpret_cast<short_t*>(reg[rid2].word + word);
				break;

			case STORE_S:
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				program.read<reg_t>(&rid2);
				*reinterpret_cast<short_t*>(reg[rid1].word + word) = reg[rid2].short_;
				break;

			case JMP: 
				program.read<word_t>(&word);
				program.goto_(word);
				break;

			case JMP_Z: 
				if (reg[register_::FZ].word) {
					program.ip += sizeof(word_t);
				} else {
					program.read<word_t>(&word);
					program.goto_(word);
				}
				break;

			case JMP_NZ: 
				if (reg[register_::FZ].word) {
					program.read<word_t>(&word);
					program.goto_(word);
				} else {
					program.ip += sizeof(word_t);
				}
				break;

			case I_FLAG: 
				program.read<reg_t>(&rid1);
				reg[register_::FZ].word = reg[rid1].int_ == 0 ? 0 : 1;
				// TODO : Set other flags if they exist?
				break;

			case I_CMP_EQ: break;

			case I_CMP_NE: break;

			case I_CMP_GT: break;

			case I_CMP_LT: break;

			case I_CMP_GE: break;

			case I_CMP_LE: break;

			case I_INC: break;

			case I_DEC: break;

			case I_ADD: break;

			case I_SUB: break;

			case I_MUL: break;

			case I_DIV: break;

			case I_MOD: break;

			case GLOBAL_W: break;

			case GLOBAL_B: break;

			case GLOBAL_S: break;




			default:
				throw ExecutorException(ExecutorException::UNKNOWN_OPCODE, program.ip - program.start);
				break;
		}
	}

	return 0;
}