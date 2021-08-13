namespace vm {
	namespace register_ {
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Register | ID		| Desc
		// -		| IP		| Instruction pointer
		// 0		| BP		| Stack base pointer
		// 1		| PP		| Program memory pointer (points to the base of the binary file loaded into memory)
		// 2		| FZ		| Zero flag (set automatically by arithmetic operations, zero if the result is zero, one otherwise)
		// 3 ... 31	| R0 .. 28	| General purpose

		enum {
			PP,// 0
			BP,// 1
			FZ,// 2
			R0 // 3
		};
		
		constexpr const char* const strings[] = {
			"PP",
			"BP",
			"FZ"
		};

		constexpr int NUM_GEN_REGISTERS = 29;
	}
}