#include "executor.h"
#include <limits>
#include <math.h>

using std::cout;

#define AS_WORD(x) \
	reinterpret_cast<types::word_t*>(x)

int vm::executor::exec(const char* const& path, ExecutorSettings& execSettings) {
	cout << "Attempting to execute file \"" << path << "\"\n";

	std::fstream file;
	file.open(path, std::ios::in | std::ios::binary);

	try {
		return vm::executor::exec_(file, execSettings, std::cout, std::cin);
	} catch (ExecutorException& e) {
		cout << IO_ERR "Error during execution at BYTE" << e.loc << " : " << e.what() << IO_NORM IO_END;
	} catch (std::exception& e) {
		cout << IO_ERR "An unknown error ocurred during execution. This error is most likely an issue with the c++ executor code, not your code. Sorry. The provided error message is as follows:\n" << e.what() << IO_NORM IO_END;
	}

	return 1;
}

int vm::executor::exec_(std::iostream& file, ExecutorSettings& execSettings, std::ostream& streamOut, std::istream& streamIn) {
	using namespace types;
	using namespace opcode;

	Program program(file);
	Stack stack(execSettings.stackSize);
	WordVal wordReg[register_::COUNT];
	ByteVal byteReg[register_::COUNT];
	wordReg[register_::PP].word = reinterpret_cast<word_t>(program.start);
	wordReg[register_::BP].word = reinterpret_cast<word_t>(stack.start);
	byteReg[register_::FZ].bool_ = 0;

	program.goto_(format::FIRST_INSTR_ADDR_LOCATION);
	program.goto_(*AS_WORD(program.ip));

	// Dummy values
	opcode_t opcode = 0;
	reg_t rid1 = 0;
	reg_t rid2 = 0;
	reg_t rid3 = 0;
	word_t word = 0;
	byte_t byte = 0;
	int_t int_ = 0;
	char_t char_ = 0;
	float_t float_ = 0;
	char rlchar = 0;

	while (program.ip < program.end) {
		program.read<opcode_t>(&opcode);
		switch (opcode) {
			case NOP:
				break;

			case HALT:
				goto end;
				return 0;

			case BREAK:
				while (streamIn.get() != '\n');
				break;

			case ALLOC: // TODO : Careful with the memory!
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				try {
					wordReg[rid1].word = reinterpret_cast<word_t>(new char[wordReg[rid2].word]);
				} catch (std::bad_alloc& e) {
					throw ExecutorException(ExecutorException::BAD_ALLOC, program.ip - program.start, e.what());
				}
				break;

			case FREE:
				program.read<reg_t>(&rid1);
				delete[] reinterpret_cast<char*>(wordReg[rid1].word);
				break;

			case R_MOV_W:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				wordReg[rid1] = wordReg[rid2];
				break;

			case R_MOV_B:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[rid1] = byteReg[rid2];
				break;

			case MOV_W:
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				wordReg[rid1].word = word;
				break;

			case MOV_B:
				program.read<reg_t>(&rid1);
				program.read<byte_t>(&byte);
				byteReg[rid1].byte = byte;
				break;

			case LOAD_W:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<word_t>(&word);
				wordReg[rid1].word = *reinterpret_cast<word_t*>(wordReg[rid2].word + word);
				break;

			case STORE_W:
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				program.read<reg_t>(&rid2);
				*reinterpret_cast<word_t*>(wordReg[rid1].word + word) = wordReg[rid2].word;
				break;

			case LOAD_B:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<word_t>(&word);
				byteReg[rid1].byte = *reinterpret_cast<byte_t*>(wordReg[rid2].word + word);
				break;

			case STORE_B:
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				program.read<reg_t>(&rid2);
				*reinterpret_cast<byte_t*>(wordReg[rid1].word + word) = byteReg[rid2].byte;
				break;

			case JMP:
				program.read<word_t>(&word);
				program.goto_(word);
				break;

			case JMP_Z:
				if (byteReg[register_::FZ].bool_) {
					program.ip += sizeof(word_t);
				} else {
					program.read<word_t>(&word);
					program.goto_(word);
				}
				break;

			case JMP_NZ:
				if (byteReg[register_::FZ].bool_) {
					program.read<word_t>(&word);
					program.goto_(word);
				} else {
					program.ip += sizeof(word_t);
				}
				break;

			case R_JMP:
				program.read<reg_t>(&rid1);
				program.goto_(wordReg[rid1].word);
				break;

			case R_JMP_Z:
				if (byteReg[register_::FZ].bool_ == 0) {
					program.ip += sizeof(reg_t);
				} else {
					program.read<reg_t>(&rid1);
					program.goto_(wordReg[rid1].word);
				}
				break;

			case R_JMP_NZ:
				if (byteReg[register_::FZ].bool_ == 0) {
					program.read<reg_t>(&rid1);
					program.goto_(wordReg[rid1].word);
				} else {
					program.ip += sizeof(reg_t);
				}
				break;

			case I_FLAG:
				program.read<reg_t>(&rid1);
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == 0 ? 0 : 1;
				// TODO : Set other flags if they exist?
				break;

			case I_CMP_EQ:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == wordReg[rid2].int_ ? 1 : 0;
				break;

			case I_CMP_NE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ != wordReg[rid2].int_ ? 1 : 0;
				break;

			case I_CMP_GT:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ > wordReg[rid2].int_ ? 1 : 0;
				break;

			case I_CMP_LT:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ < wordReg[rid2].int_ ? 1 : 0;
				break;

			case I_CMP_GE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ >= wordReg[rid2].int_ ? 1 : 0;
				break;

			case I_CMP_LE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ <= wordReg[rid2].int_ ? 1 : 0;
				break;

			case I_INC:
				program.read<reg_t>(&rid1);
				wordReg[rid1].int_++;
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == 0 ? 0 : 1;
				break;

			case I_DEC:
				program.read<reg_t>(&rid1);
				wordReg[rid1].int_--;
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == 0 ? 0 : 1;
				break;

			case I_ADD:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				wordReg[rid1].int_ = wordReg[rid2].int_ + wordReg[rid3].int_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == 0 ? 0 : 1;
				break;

			case I_SUB:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				wordReg[rid1].int_ = wordReg[rid2].int_ - wordReg[rid3].int_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == 0 ? 0 : 1;
				break;

			case I_MUL:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				wordReg[rid1].int_ = wordReg[rid2].int_ * wordReg[rid3].int_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == 0 ? 0 : 1;
				break;

			case I_DIV:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				if (wordReg[rid3].int_ == 0) throw ExecutorException(ExecutorException::DIVIDE_BY_ZERO, program.ip - program.start);
				wordReg[rid1].int_ = wordReg[rid2].int_ / wordReg[rid3].int_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == 0 ? 0 : 1;
				break;

			case I_MOD:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				if (wordReg[rid3].int_ == 0) throw ExecutorException(ExecutorException::DIVIDE_BY_ZERO, program.ip - program.start);
				wordReg[rid1].int_ = wordReg[rid2].int_ % wordReg[rid3].int_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].int_ == 0 ? 0 : 1;
				break;

			case I_TO_C:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[rid1].char_ = static_cast<char_t>(wordReg[rid2].int_);
				break;

			case I_TO_F:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				wordReg[rid1].float_ = static_cast<float_t>(wordReg[rid2].int_);
				break;

			case C_FLAG:
				program.read<reg_t>(&rid1);
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == 0 ? 0 : 1;
				// TODO : Set other flags if they exist?
				break;

			case C_CMP_EQ:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == byteReg[rid2].char_ ? 1 : 0;
				break;

			case C_CMP_NE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ != byteReg[rid2].char_ ? 1 : 0;
				break;

			case C_CMP_GT:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ > byteReg[rid2].char_ ? 1 : 0;
				break;

			case C_CMP_LT:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ < byteReg[rid2].char_ ? 1 : 0;
				break;

			case C_CMP_GE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ >= byteReg[rid2].char_ ? 1 : 0;
				break;

			case C_CMP_LE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ <= byteReg[rid2].char_ ? 1 : 0;
				break;

			case C_INC:
				program.read<reg_t>(&rid1);
				byteReg[rid1].char_++;
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == 0 ? 0 : 1;
				break;

			case C_DEC:
				program.read<reg_t>(&rid1);
				byteReg[rid1].char_--;
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == 0 ? 0 : 1;
				break;

			case C_ADD:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				byteReg[rid1].char_ = byteReg[rid2].char_ + byteReg[rid3].char_;
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == 0 ? 0 : 1;
				break;

			case C_SUB:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				byteReg[rid1].char_ = byteReg[rid2].char_ - byteReg[rid3].char_;
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == 0 ? 0 : 1;
				break;

			case C_MUL:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				byteReg[rid1].char_ = byteReg[rid2].char_ * byteReg[rid3].char_;
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == 0 ? 0 : 1;
				break;

			case C_DIV:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				if (byteReg[rid3].char_ == 0) throw ExecutorException(ExecutorException::DIVIDE_BY_ZERO, program.ip - program.start);
				byteReg[rid1].char_ = byteReg[rid2].char_ / byteReg[rid3].char_;
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == 0 ? 0 : 1;
				break;

			case C_MOD:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				if (byteReg[rid3].char_ == 0) throw ExecutorException(ExecutorException::DIVIDE_BY_ZERO, program.ip - program.start);
				byteReg[rid1].char_ = byteReg[rid2].char_ % byteReg[rid3].char_;
				byteReg[register_::FZ].bool_ = byteReg[rid1].char_ == 0 ? 0 : 1;
				break;

			case C_TO_I:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				wordReg[rid1].int_ = static_cast<char_t>(byteReg[rid2].char_);
				break;

			case C_TO_F:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				wordReg[rid1].float_ = static_cast<float_t>(byteReg[rid2].char_);
				break;

			case F_FLAG:
				program.read<reg_t>(&rid1);
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ == 0 ? 0 : 1;
				// TODO : Set other flags if they exist?
				break;

			case F_CMP_EQ:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ == wordReg[rid2].float_ ? 1 : 0;
				break;

			case F_CMP_NE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ != wordReg[rid2].float_ ? 1 : 0;
				break;

			case F_CMP_GT:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ > wordReg[rid2].float_ ? 1 : 0;
				break;

			case F_CMP_LT:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ < wordReg[rid2].float_ ? 1 : 0;
				break;

			case F_CMP_GE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ >= wordReg[rid2].float_ ? 1 : 0;
				break;

			case F_CMP_LE:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ <= wordReg[rid2].float_ ? 1 : 0;
				break;

			case F_ADD:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				wordReg[rid1].float_ = wordReg[rid2].float_ + wordReg[rid3].float_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ == 0 ? 0 : 1;
				break;

			case F_SUB:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				wordReg[rid1].float_ = wordReg[rid2].float_ - wordReg[rid3].float_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ == 0 ? 0 : 1;
				break;

			case F_MUL:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				wordReg[rid1].float_ = wordReg[rid2].float_ * wordReg[rid3].float_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ == 0 ? 0 : 1;
				break;

			case F_DIV:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				if (wordReg[rid3].float_ == 0) throw ExecutorException(ExecutorException::DIVIDE_BY_ZERO, program.ip - program.start);
				wordReg[rid1].float_ = wordReg[rid2].float_ / wordReg[rid3].float_;
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ == 0 ? 0 : 1;
				break;

			case F_MOD:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				program.read<reg_t>(&rid3);
				if (wordReg[rid3].float_ == 0) throw ExecutorException(ExecutorException::DIVIDE_BY_ZERO, program.ip - program.start);
				wordReg[rid1].float_ = wordReg[rid2].float_ * modf(wordReg[rid2].float_ / wordReg[rid3].float_, &float_);
				byteReg[register_::FZ].bool_ = wordReg[rid1].float_ == 0 ? 0 : 1;
				break;

			case F_TO_C:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				byteReg[rid1].char_ = static_cast<char_t>(wordReg[rid2].float_);
				break;

			case F_TO_I:
				program.read<reg_t>(&rid1);
				program.read<reg_t>(&rid2);
				wordReg[rid1].int_ = static_cast<int_t>(wordReg[rid2].float_);
				break;

			case PRNT_C:
				program.read<reg_t>(&rid1);
				streamOut << byteReg[rid1].char_;
				break;

			case PRNT_STR:
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				streamOut << reinterpret_cast<char*>(wordReg[rid1].word + word);
				break;

			case READ_C:
				program.read<reg_t>(&rid1);
				streamIn.get(rlchar);
				byteReg[rid1].char_ = rlchar;
				break;

			case READ_STR:
				program.read<reg_t>(&rid1);
				program.read<word_t>(&word);
				streamIn.getline(reinterpret_cast<char*>(wordReg[rid1].word + word), std::numeric_limits<std::streamsize>::max(), '\n');
				break;

			case R_PRNT_I:
				program.read<reg_t>(&rid1);
				streamOut << wordReg[rid1].int_;
				break;

			case R_PRNT_F:
				program.read<reg_t>(&rid1);
				streamOut << wordReg[rid1].float_;
				break;

			case PRNT_LN:
				streamOut << '\n';
				break;

			default:
				throw ExecutorException(ExecutorException::UNKNOWN_OPCODE, program.ip - program.start);
				break;
		}
	}

end:;
	// TODO: automatic memory cleanup
	streamOut << IO_END;

	return 0;
}