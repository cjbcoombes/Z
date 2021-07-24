namespace vm {
	namespace register_ {
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Register | ID		| Desc
		// -		| IP		| Instruction pointer
		// 0		| BP		| Stack base pointer
		// 1		| CP/GP		| Constants/Globals memory pointer
		// 2 ... 31	| R0 .. 29	| General purpose

		enum {
			BP,// 0
			GP,// 1
			R0 // 2
		};

		constexpr int NUM_GEN_REGISTERS = 30;
	}
}