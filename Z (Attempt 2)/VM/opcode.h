namespace vm {
	namespace opcode {
		enum Opcode {
			NOP,
			HALT,
			//
			ALLOC,
			FREE,
			// Probably non-permanent:
			R_PRNT_W,
			//
			PRNT_LN,
			PRNT_C,
			PRNT_STR,
			//
			READ_STR, 
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
			I_TO_C,
			//
			C_FLAG,
			C_CMP_EQ,
			C_CMP_NE,
			C_CMP_GT,
			C_CMP_LT,
			C_CMP_GE,
			C_CMP_LE,
			//
			C_INC,
			C_DEC,
			C_ADD,
			C_SUB,
			C_MUL,
			C_DIV,
			C_MOD,
			C_TO_I,
			//
			//
			//
			GLOBAL_W,
			GLOBAL_B,
			GLOBAL_S,
			GLOBAL_STR,

			INVALID = 255
		};

		constexpr int GLOBAL_BREAK = GLOBAL_W;

		constexpr const char* const strings[] = {
			"nop",
			"halt",
			//
			"alloc",
			"free",
			// 
			"rprntw",
			//
			"prntln",
			"prntc",
			"prntstr",
			//
			"readstr",
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
			"itoc",
			//
			"cflag",
			"ccmpeq",
			"ccmpne",
			"ccmpgt",
			"ccmplt",
			"ccmpge",
			"ccmple",
			//
			"cinc",
			"cdec",
			"cadd",
			"csub",
			"cmul",
			"cdiv",
			"cmod",
			"ctoi",
			//
			//
			//
			"globalw",
			"globalb",
			"globals",
			"globalstr"
		};

		constexpr int count = sizeof(strings) / sizeof(strings[0]);

		constexpr int MAX_ARGS = 3;
		enum class ArgType {
			ARG_NONE,	// 0
			ARG_REG,	// 1
			ARG_WORD,	// 2 (Also processes labels)
			ARG_BYTE,	// 3
			ARG_SHORT,	// 4
			ARG_VAR,	// 5 (Only for setting vars, use ARG_WORD for reading them)
			ARG_STR		// 6
		};
		constexpr int args[][MAX_ARGS] = {
			{0, 0, 0},	// NOP
			{0, 0, 0},	// HALT
			//
			{1, 1, 0},	// ALLOC
			{1, 0, 0},	// FREE
			//
			{1, 0, 0},	// R_PRNT_W
			//
			{0, 0, 0},	// PRNT_LN
			{1, 0, 0},	// PRNT_C
			{1, 2, 0},	// PRNT_STR
			//
			{1, 0, 0},	// READ_STR
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
			{1, 1, 0},	// I_TO_C
			//
			{1, 0, 0},	// C_FLAG
			{1, 1, 0},	// C_CMP_EQ
			{1, 1, 0},	// C_CMP_NE
			{1, 1, 0},	// C_CMP_GT
			{1, 1, 0},	// C_CMP_LT
			{1, 1, 0},	// C_CMP_GE
			{1, 1, 0},	// C_CMP_LE
			//
			{1, 0, 0},	// C_INC
			{1, 0, 0},	// C_DEC
			{1, 1, 1},	// C_ADD
			{1, 1, 1},	// C_SUB
			{1, 1, 1},	// C_MUL
			{1, 1, 1},	// C_DIV
			{1, 1, 1},	// C_MOD
			{1, 1, 0},	// C_TO_I
			//
			// 
			//
			{5, 2, 0},	// GLOBAL_W
			{5, 3, 0},	// GLOBAL_B
			{5, 4, 0},	// GLOBAL_S
			{5, 6, 0},	// GLOBAL_STR
		};
	}
}