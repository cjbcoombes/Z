namespace vm {
	namespace opcode {
		enum Opcode {
			NOP,
			HALT,
			// Probably non-permanent:
			R_PRNT_W,
			LN_PRNT,
			//
			//IF_Z,
			//IF_NZ,
			//
			MOV,
			MOV_W,
			MOV_B,
			MOV_S,
			//
			LOAD_W,
			STORE_W,
			LOAD_B,
			STORE_B,
			LOAD_S,
			STORE_S,
			//
			JMP,
			JMP_Z,
			JMP_NZ,
			R_JMP,
			R_JMP_Z,
			R_JMP_NZ,
			//
			I_FLAG,
			I_CMP_EQ,
			I_CMP_NE,
			I_CMP_GT,
			I_CMP_LT,
			I_CMP_GE,
			I_CMP_LE,
			//
			I_INC,
			I_DEC,
			I_ADD,
			I_SUB,
			I_MUL,
			I_DIV,
			I_MOD,
			//
			//
			//
			GLOBAL_W,
			GLOBAL_B,
			GLOBAL_S,

			INVALID = 255
		};

		constexpr int GLOBAL_BREAK = GLOBAL_W;

		constexpr const char* const strings[] = {
			"nop",
			"halt",
			// 
			"rprntw",
			"lnprnt",
			//
			//"ifz",
			//"ifnz",
			//
			"mov",
			"movw",
			"movb",
			"movs",
			//
			"loadw",
			"storew",
			"loadb",
			"storeb",
			"loads",
			"stores",
			//
			"jmp",
			"jmpz",
			"jmpnz",
			"rjmp",
			"rjmpz",
			"rjmpnz",
			//
			"iflag",
			"icmpeq",
			"icmpne",
			"icmpgt",
			"icmplt",
			"icmpge",
			"icmple",
			//
			"iinc",
			"idec",
			"iadd",
			"isub",
			"imul",
			"idiv",
			"imod",
			//
			//
			//
			"globalw",
			"globalb",
			"globals"
		};

		constexpr int count = sizeof(strings) / sizeof(strings[0]);

		constexpr int MAX_ARGS = 3;
		enum class ArgType {
			ARG_NONE,	// 0
			ARG_REG,	// 1
			ARG_WORD,	// 2 (Also processes labels)
			ARG_BYTE,	// 3
			ARG_SHORT,	// 4
			ARG_VAR		// 5 (Only for setting vars, use ARG_WORD for reading them)
		};
		constexpr int args[][MAX_ARGS] = {
			{0, 0, 0},	// NOP
			{0, 0, 0},	// HALT
			//
			{1, 0, 0}, // R_PRNT_W
			{0, 0, 0}, // LN_PRNT
			//
			//{0, 0, 0},	// IF_Z
			//{0, 0, 0},	// IF_NZ
			//
			{1, 1, 0},	// MOV
			{1, 2, 0},	// MOV_W
			{1, 3, 0},	// MOV_B
			{1, 4, 0},	// MOV_S
			//			
			{1, 1, 2},	// LOAD_W
			{1, 2, 1},	// STORE_W
			{1, 1, 2},	// LOAD_B
			{1, 2, 1},	// STORE_B
			{1, 1, 2},	// LOAD_S
			{1, 2, 1},	// STORE_S
			//
			{2, 0, 0},	// JMP
			{2, 0, 0},	// JMP_Z
			{2, 0, 0},	// JMP_NZ
			{1, 0, 0},	// R_JMP
			{1, 0, 0},	// R_JMP_Z
			{1, 0, 0},	// R_JMP_NZ
			//
			{1, 0, 0},	// I_FLAG
			{1, 1, 0},	// I_CMP_EQ
			{1, 1, 0},	// I_CMP_NE
			{1, 1, 0},	// I_CMP_GT
			{1, 1, 0},	// I_CMP_LT
			{1, 1, 0},	// I_CMP_GE
			{1, 1, 0},	// I_CMP_LE
			//
			{1, 0, 0},	// I_INC
			{1, 0, 0},	// I_DEC
			{1, 1, 1},	// I_ADD
			{1, 1, 1},	// I_SUB
			{1, 1, 1},	// I_MUL
			{1, 1, 1},	// I_DIV
			{1, 1, 1},	// I_MOD
			//
			// 
			//
			{5, 2, 0},	// GLOBAL_W
			{5, 3, 0},	// GLOBAL_B
			{5, 4, 0}	// GLOBAL_S
		};
	}
}