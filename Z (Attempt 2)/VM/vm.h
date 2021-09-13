#pragma once

#include "..\utils.h"
#include "opcode.h"
#include "register.h"

namespace vm {

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constants?
	constexpr types::word_t wordPlaceholder = 0xbcbcbcbc;
	constexpr types::word_t wordErr = 0xecececec;
	constexpr char charFiller = opcode::HALT;

	namespace format {
		constexpr int FIRST_INSTR_ADDR_LOCATION = 0;
		constexpr int GLOBAL_TABLE_LOCATION = 4;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Opcodes: Included in separate header file


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Registers: Included in separate header file


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Assembler: Included in separate header file

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Executor: Included in separate header file
	
}