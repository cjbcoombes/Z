#pragma once
#define THROW(x) \
	throw vm::AssemblyError(vm::AssemblyError::x)

#define TO_CHAR(x) \
	reinterpret_cast<char*>(&(x))

// #ifndef VM_ASSEMBLE_INCLUDED
template<typename T>
bool parseNumber(T& num, char* str) {
	static std::stringstream convStream;
	static int16_t convChar;
	convStream.str("");
	convStream.clear();
	convStream << std::hex << str;
	if (sizeof(T) == 1) {
		if (!(convStream >> convChar))
			return false;
		num = static_cast<T>(convChar);
		return true;
	} else {
		return static_cast<bool>(convStream >> num);
	}
}

void parseOpcode(vm::types::opcode_t& opcode, char* const& str) {
	using namespace vm::Opcode;
	const char* curr;
	int j;
	for (opcode = 0; opcode < count; opcode++) {
		curr = strings[opcode];
		j = 0;
		while (true) {
			if (str[j] != curr[j])
				goto next;

			if (str[j++] == '\0')
				return;
		}
	next:;
	}

	THROW(UNKNOWN_OPCODE);
}

void parseRegister(vm::types::register_t& reg, char* const& str) {
	if (str[0] == 'R') {
		if (!parseNumber<vm::types::register_t>(reg, str + 1)) {
			THROW(INVALID_REGISTER);
		}
	} else if (str[0] == 'B' && str[1] == 'P' && str[2] == '\0') {
		reg = vm::Register::BP;
	}
}

void parseLiteral(vm::types::literal_t& lit, char* const& str) {
	if (!parseNumber<vm::types::literal_t>(lit, str)) {
		THROW(INVALID_LITERAL);
	}
}