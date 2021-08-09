namespace vm {
	namespace register_ {
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Register | ID		| Desc
		// -		| IP		| Instruction pointer
		// 0		| BP		| Stack base pointer
		// 1		| PP		| Program memory pointer (points to the base of the binary file loaded into memory)
		// 2 ... 31	| R0 .. 29	| General purpose

		enum {
			PP,// 0
			BP,// 1
			R0 // 2
		};

		constexpr int NUM_GEN_REGISTERS = 30;
	}
}