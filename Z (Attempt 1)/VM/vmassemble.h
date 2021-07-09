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
	std::string stdstr;
	int len = 0;
	char c;

	std::unordered_map<std::string, Label> labels;
	std::unordered_map<std::string, short_t> vars;
	std::string startstr = "@__START__";
	labels[startstr] = Label(0);
	labels[startstr].refs.push_back(4);


	opcode_t opcode = NOP;
	opcode_t tempOpcode = NOP;
	register_t reg = 0;
	register_t regGP = vm::Register::GP;
	word_t word = 0;
	byte_t byte = 0;
	short_t short_ = 0;
	word_t placeholderWord = 0xadadadad;
	int argc = 0;

	bool end = false;
	bool isComment = false;
	bool isGlobalSet = true;
	bool isStr = false;
	bool isEscaped = false;
	int pCount = 0;

	word_t byteCounter = 0;
	asm_.seekg(0);
	exe.seekp(0);

#ifdef VM_DEBUG
	debug << STRM_DEFAULT << "[DEBUG] Bytecode assembly debug:";
#endif

	byteCounter = 2048; // Temporarily used for stack size
	WRITE(byteCounter, word_t);
	byteCounter = 4;
	WRITE(byteCounter, word_t);// First instruction address
	byteCounter = 8;
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
			} else if (c == '(') {
				pCount++;
			} else if (c == ')') {
				if (!(pCount--)) {
					ASM_THROW(UNBALANCED_PARENS);
				}
				continue;
			}
		}
		/**/
		if (isStr) {
			if (end) {
				isStr = false;
			} else if (isEscaped) {
				switch (c) {
					case 'n': 
						c = '\n';
						break;
					case 't':
						c = '\t';
						break;
				}
				isEscaped = false;
			} else if (c == '\\') {
				if (isStr && !isEscaped) {
					isEscaped = true;
					continue;
				}
			} else if (c == '"') {
				isStr = false;
				continue;
			}
		} else if (!end && pCount == 0 && !isComment && c == '"') {
			isStr = true;
			continue;
		}

		if (pCount < 0) {
			ASM_THROW(UNBALANCED_PARENS);
		} else if (pCount > 0 || isComment) {
			continue;
		}

		if ((!isStr && (c == ' ' || c == ',' || c == '\n' || c == '\t')) || end) {
			if (len == 0)
				continue;
			str[len++] = '\0';

			if (opcode == NOP || args[opcode][argc] == ArgType::ARG_NONE) {
			#ifdef VM_DEBUG
				debug << '\n';
			#endif
				if (str[0] == '@') {
					stdstr.assign(str);
					if (!hasKey(labels, stdstr)) {
						labels[stdstr] = Label();
					}
					labels[stdstr].addr = byteCounter;
				#ifdef VM_DEBUG
					debug << str << ' ';
				#endif
				} else {

					parseOpcode(opcode, str);

					if (isGlobalSet) {
						if (opcode < GLOBAL_OPCODE_BREAK) {
							isGlobalSet = false;
							labels[startstr].addr = byteCounter;
						}
					} else {
						if (opcode > GLOBAL_OPCODE_BREAK) {
							ASM_THROW(INVALID_GLOBAL);
						}
					}

					switch (opcode) {
						case LOADGW:
							tempOpcode = LOADW;
							break;
						case STOREGW:
							tempOpcode = STOREW;
							break;
						case LOADGB:
							tempOpcode = LOADB;
							break;
						case STOREGB:
							tempOpcode = STOREB;
							break;
						case STRPRNTG:
							tempOpcode = STRPRNT;
							break;
						default:
							tempOpcode = opcode;
					}
					if (tempOpcode < VALID_OPCODE_BREAK) {
						WRITE(tempOpcode, opcode_t);
					}
					argc = 0;

				#ifdef VM_DEBUG
					debug << vm::Opcode::strings[opcode] << ' ';
				#endif
				}
			} else {
				switch (args[opcode][argc]) {
					case ArgType::ARG_NONE:
						argc = MAX_ARGS;
						break;

					case ArgType::ARG_REG:
						parseRegister(reg, str);
						WRITE(reg, register_t);
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;

					case ArgType::ARG_WORD:
						if (str[0] != '@') {
							parseWord(word, str);
							WRITE(word, word_t);
						#ifdef VM_DEBUG
							debug << str << ' ';
						#endif
							break;
						}
						// NOTE: INTENTIONAL FALLTHROUGH
					case ArgType::ARG_LABEL:
						stdstr.assign(str);
						if (!hasKey(labels, stdstr)) {
							labels[stdstr] = Label();
						}
						labels[stdstr].refs.push_back(exe.tellp());
						WRITE(placeholderWord, word_t);
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;

					case ArgType::ARG_BYTE:
						parseByte(byte, str);
						WRITE(byte, byte_t);
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;

					case ArgType::ARG_SHORT:
						parseShort(short_, str);
						WRITE(short_, short_t);
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;

					case ArgType::ARG_VAR:
						stdstr.assign(str);
						if (opcode > GLOBAL_OPCODE_BREAK) {
							vars[stdstr] = byteCounter;
						} else {
							if (!hasKey(vars, stdstr)) {
								ASM_THROW(UNDEFINED_VAR);
							}
							WRITE(regGP, register_t);
							WRITE(vars[stdstr], short_t);
						}
					#ifdef VM_DEBUG
						debug << str << ' ';
					#endif
						break;

					case ArgType::ARG_STR:
						WRITE_N(str, len);
					#ifdef VM_DEBUG
						debug << '"' << str << "\" ";
					#endif
						break;
				}
				if (++argc >= MAX_ARGS) {
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

	for (const std::pair<std::string, Label>& label : labels) {
		for (const std::streampos& pos : label.second.refs) {
			exe.seekp(pos);
			exe.write(reinterpret_cast<char*>(const_cast<word_t*>(&label.second.addr)), sizeof(word_t));
		}
	}

#ifdef VM_DEBUG
	debug << '\n';
#endif
}