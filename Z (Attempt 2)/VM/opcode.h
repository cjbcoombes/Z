namespace vm {
	namespace opcode {
		enum Opcode {
			NOP,
			HALT,
			// Probably non-permanent:
			RPRNT,
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
			//
			I_FLAG,
			I_CMP_ALL,
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



			INVALID = 255
		};

		constexpr const char* const strings[] = {
			"nop",
			"halt",
			// 
			"rprnt",
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
			//
			"iflag",
			"icmpall",
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
			"imod"
		};

		constexpr int count = sizeof(strings) / sizeof(strings[0]);

		constexpr int MAX_ARGS = 3;
		enum class ArgType {
			ARG_NONE,	// 0
			ARG_REG,	// 1
			ARG_WORD,	// 2 (Also processes labels)
			ARG_BYTE,	// 3
			ARG_SHORT,	// 4
		};
		constexpr int args[][MAX_ARGS] = {
			{0, 0, 0},	// NOP
			{0, 0, 0},	// HALT
			//
			{1, 0, 0}, // RPRNT
			//
			//{0, 0, 0},	// IF_Z
			//{0, 0, 0},	// IF_NZ
			//
			{1, 1, 0},	// MOV
			{1, 2, 0},	// MOV_W
			{1, 3, 0},	// MOV_B
			{1, 4, 0},	// MOV_S
			//			
			{1, 1, 4},	// LOAD_W
			{1, 4, 1},	// STORE_W
			{1, 1, 4},	// LOAD_B
			{1, 4, 1},	// STORE_B
			{1, 1, 4},	// LOAD_S
			{1, 4, 1},	// STORE_S
			//			
			{2, 0, 0},	// JMP
			//
			{1, 0, 0},	// I_FLAG
			{1, 1, 0},	// I_CMP_ALL
			{1, 1, 0},	// I_CMP_EQ
			{1, 1, 0},	// I_CMP_NE
			{1, 1, 0},	// I_CMP_GT
			{1, 1, 0},	// I_CMP_LT
			{1, 1, 0},	// I_CMP_GE
			{1, 1, 0},	// I_CMP_LE
			//
			{1, 0, 0},	// IINC
			{1, 0, 0},	// IDEC
			{1, 1, 1},	// IADD
			{1, 1, 1},	// ISUB
			{1, 1, 1},	// IMUL
			{1, 1, 1},	// IDIV
			{1, 1, 1}	// IMOD
		};
	}
}