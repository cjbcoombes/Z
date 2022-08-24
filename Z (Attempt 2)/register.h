namespace register_ {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Register | ID		| Desc
	// -		| IP		| Instruction pointer
	// 0		| BP		| Stack base pointer
	// 1		| PP		| Program memory pointer (points to the base of the binary file loaded into memory)
	// 2		| FZ		| Zero flag (set automatically by arithmetic operations, zero if the result is zero, one otherwise)
	// 3 ... 17	| W0 .. 14	| General purpose word
	// 18 .. 31	| B0 .. 13	| General purpose byte

	enum {
		PP = 0,	// 0
		BP = 1,	// 1
		FZ = 2,	// 2
		W0 = 3,	// 3
		B0 = 18,
		COUNT = 32
	};

	constexpr const char* const strings[] = {
		"PP",
		"BP",
		"FZ"
	};

	constexpr int NUM_WORD_REGISTERS = 15;
	constexpr int NUM_BYTE_REGISTERS = 14;
}