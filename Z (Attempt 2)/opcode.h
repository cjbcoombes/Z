namespace opcode {
	enum Opcode {
		NOP,
		HALT,
		BREAK,
		//
		ALLOC,
		FREE,
		//
		R_MOV_W,
		R_MOV_B,
		MOV_W,
		MOV_B,
		//
		LOAD_W,
		STORE_W,
		LOAD_B,
		STORE_B,
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
		I_TO_F,
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
		C_TO_F,
		//
		F_FLAG,
		F_CMP_EQ,
		F_CMP_NE,
		F_CMP_GT,
		F_CMP_LT,
		F_CMP_GE,
		F_CMP_LE,
		//
		F_ADD,
		F_SUB,
		F_MUL,
		F_DIV,
		F_MOD,
		F_TO_I,
		F_TO_C,
		//
		// 
		//
		PRNT_C,
		PRNT_STR,
		//
		READ_C,
		READ_STR,
		//
		R_PRNT_I,
		R_PRNT_F,
		PRNT_LN,
		//
		TIME,
		// 
		//
		//
		GLOBAL_W,
		GLOBAL_B,
		GLOBAL_STR,

		INVALID = 255
	};

	constexpr int SYSCALL_BREAK = PRNT_C;
	constexpr int DEBUG_BREAK = R_PRNT_I;
	constexpr int GLOBAL_BREAK = GLOBAL_W;

	constexpr const char* const strings[] = {
		"nop",
		"halt",
		"break",
		//
		"alloc",
		"free",
		//
		"rmovw",
		"rmovb",
		"movw",
		"movb",
		//
		"loadw",
		"storew",
		"loadb",
		"storeb",
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
		"itof",
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
		"ctof",
		//
		"fflag",
		"fcmpeq",
		"fcmpne",
		"fcmpgt",
		"fcmplt",
		"fcmpge",
		"fcmple",
		//
		"fadd",
		"fsub",
		"fmul",
		"fdiv",
		"fmod",
		"ftoi",
		"ftoc",
		//
		// 
		// 
		"prntc",
		"prntstr",
		//
		"readc",
		"readstr",
		// 
		"rprnti",
		"rprntf",
		"prntln",
		//
		"time",
		// 
		//
		//
		"globalw",
		"globalb",
		"globalstr"
	};

	constexpr int count = sizeof(strings) / sizeof(strings[0]);

	constexpr int MAX_ARGS = 3;
	enum class ArgType {
		ARG_NONE,		// 0
		ARG_WORD_REG,	// 1
		ARG_BYTE_REG,	// 2
		ARG_WORD,		// 3 (Also processes labels)
		ARG_BYTE,		// 4
		ARG_VAR,		// 5 (Only for setting vars, use ARG_WORD for reading them)
		ARG_STR			// 6
	};
	constexpr int args[][MAX_ARGS] = {
		{0, 0, 0},	// NOP
		{0, 0, 0},	// HALT
		{0, 0, 0},	// BREAK
		//
		{1, 1, 0},	// ALLOC
		{1, 0, 0},	// FREE
		//
		{1, 1, 0},	// R_MOV_W
		{2, 2, 0},	// R_MOV_B
		{1, 3, 0},	// MOV_W
		{2, 4, 0},	// MOV_B
		//			
		{1, 1, 3},	// LOAD_W
		{1, 3, 1},	// STORE_W
		{2, 1, 3},	// LOAD_B
		{1, 3, 2},	// STORE_B
		//
		{3, 0, 0},	// JMP
		{3, 0, 0},	// JMP_Z
		{3, 0, 0},	// JMP_NZ
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
		{2, 1, 0},	// I_TO_C
		{1, 1, 0},	// I_TO_F
		//
		{2, 0, 0},	// C_FLAG
		{2, 2, 0},	// C_CMP_EQ
		{2, 2, 0},	// C_CMP_NE
		{2, 2, 0},	// C_CMP_GT
		{2, 2, 0},	// C_CMP_LT
		{2, 2, 0},	// C_CMP_GE
		{2, 2, 0},	// C_CMP_LE
		//
		{2, 0, 0},	// C_INC
		{2, 0, 0},	// C_DEC
		{2, 2, 2},	// C_ADD
		{2, 2, 2},	// C_SUB
		{2, 2, 2},	// C_MUL
		{2, 2, 2},	// C_DIV
		{2, 2, 2},	// C_MOD
		{1, 2, 0},	// C_TO_I
		{1, 2, 0},	// C_TO_F
		//
		{1, 0, 0},	// F_FLAG
		{1, 1, 0},	// F_CMP_EQ
		{1, 1, 0},	// F_CMP_NE
		{1, 1, 0},	// F_CMP_GT
		{1, 1, 0},	// F_CMP_LT
		{1, 1, 0},	// F_CMP_GE
		{1, 1, 0},	// F_CMP_LE
		//
		{1, 1, 1},	// F_ADD
		{1, 1, 1},	// F_SUB
		{1, 1, 1},	// F_MUL
		{1, 1, 1},	// F_DIV
		{1, 1, 1},	// F_MOD
		{2, 1, 0},	// F_TO_C
		{1, 1, 0},	// F_TO_I
		//
		// 
		//
		{2, 0, 0},	// PRNT_C
		{1, 3, 0},	// PRNT_STR
		//
		{2, 0, 0},	// READ_C
		{1, 3, 0},	// READ_STR
		//
		{1, 0, 0},	// R_PRNT_I
		{1, 0, 0},	// R_PRNT_F
		{0, 0, 0},	// PRNT_LN
		//
		{1, 0, 0},	// TIME
		// 
		// 
		//
		{5, 3, 0},	// GLOBAL_W
		{5, 4, 0},	// GLOBAL_B
		{5, 6, 0},	// GLOBAL_STR
	};
}