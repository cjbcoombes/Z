namespace register_ {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Register | ID		| Desc
	// -		| IP		| Instruction pointer
	// 0		| BP		| Stack base pointer (moves with the current stack frame)
	// 1		| RP		| Stack root pointer (always the start of the stack, where global variables are)
	// 2		| PP		| Program memory pointer (points to the base of the binary file loaded into memory)
	// 3		| FZ		| Zero flag (set automatically by arithmetic operations, zero if the result is zero, one otherwise)
	// 4 ... 17	| W0 .. 13	| General purpose word
	// 18 .. 31	| B0 .. 13	| General purpose byte

	enum {
		BP = 0,	// 0
		RP = 1, // 1
		PP = 2,	// 2
		FZ = 3,	// 3
		W0 = 4,	// 4
		B0 = 18,
		COUNT = 32
	};

	constexpr const char* const strings[] = {
		"BP",
		"RP",
		"PP",
		"FZ"
	};

	constexpr int NUM_WORD_REGISTERS = 14;
	constexpr int NUM_BYTE_REGISTERS = 14;
}