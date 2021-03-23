// Utilities for the vm
#pragma once
#include <exception>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
static_assert(sizeof(void*) == 4, "No workaround for non-32-bit pointers");
#define VM_DEBUG

namespace vm {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Typedefs

	// Types and sizes of various items
	namespace types {
		typedef uint8_t register_t;
		typedef uint8_t opcode_t;
		typedef uint32_t literal_t;
		typedef int8_t offset_t;
		typedef uint32_t address_t;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Custom exceptions
	
	// Error during assembly
	class AssemblyError : public std::exception {
	public:

		// Error type enum
		const enum ErrorType {
			UNKNOWN_ERROR,
			MAX_STR_SIZE_REACHED,
			UNKNOWN_OPCODE,
			UNBALANCED_PARENS,
			INVALID_REGISTER,
			INVALID_LITERAL
		};

		static constexpr const char* const test[] = { 
			"Unknown error",
			"The maximum size of a contiguous string (no whitespace) has been reached",
			"Unknown opcode",
			"Unbalanced parentheses"
		};

		const ErrorType type;
	#pragma warning( push )
	#pragma warning( disable: 26812 ) // Prefer 'enum class' over unscoped 'enum'
		AssemblyError(ErrorType eType) : type(eType) {}
	#pragma warning( pop )

		virtual const char* what() const {
			return test[type];
		}
	};

	// Error during execution
	class ExecutionError : public std::exception {
	public:

		// Error type enum
		const enum ErrorType {
			UNKNOWN_ERROR
		};

		static constexpr const char* const test[] = {
			"Unknown error"
		};

		const ErrorType type;
	#pragma warning( push )
	#pragma warning( disable: 26812 ) // Prefer 'enum class' over unscoped 'enum'
		ExecutionError(ErrorType eType) : type(eType) {}
	#pragma warning( pop )

		virtual const char* what() const {
			return test[type];
		}
	};

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## File buffer

	// A buffer for loading binary files into memory to increase execution speed
	class FileBuffer {
	public:
		static const unsigned int MAX_BUFFER_SIZE = UINT_MAX;

		char* start;
		char* ptr;
		char* end;
		unsigned int size;

		FileBuffer(std::fstream& file) {
			// https://stackoverflow.com/questions/22984956/tellg-function-give-wrong-size-of-file/22986486#22986486
			file.clear();
			file.seekg(0, std::ios_base::beg);
			file.ignore(std::numeric_limits<std::streamsize>::max());
		#pragma warning( push )
		#pragma warning( disable: 4244 )// Lossy conversion from std::streamsize to unsigned int
			size = file.gcount();
		#pragma warning( pop )
			if (size > MAX_BUFFER_SIZE) 
				size = MAX_BUFFER_SIZE;

			file.clear();
			file.seekg(0, std::ios_base::beg);

			start = new char[size];
			ptr = start;
			end = start + size;
			file.read(start, size);
		}

		virtual ~FileBuffer() {
			delete[] start;
		}

		template<typename T>
		void read(T* val) {
			(*val) = *reinterpret_cast<T*>(ptr);
			ptr += sizeof(T);
		}
	};

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Opcodes

	// Use of namespace instead of enum class allows for implicit casting
	namespace Opcode {
		// Enum for opcodes
		enum {
			NOP,
			//
			HALT,
			//
			MOV,
			MOVL,
			//
			LOAD,
			STORE,
			//
			JMP,
			FJMP
		};

		// Strings for opcodes
		const char* const strings[] = {
			"nop",
			//
			"halt",
			//
			"mov",
			"movl",
			//
			"load",
			"store",
			//
			"jmp",
			"fjmp"
		};

		constexpr int MAX_ARGS = 3;
		namespace ArgType {
			enum {
				ARG_NONE,	// 0
				ARG_REG,	// 1
				ARG_LIT32,	// 2
				ARG_ADDR,	// 3
				ARG_OFF,	// 4
			};
		}
		const int args[][MAX_ARGS] = {
			{0, 0, 0},// NOP
			//
			{0, 0, 0},// HALT
			//
			{1, 1, 0},// MOV
			{1, 2, 0},// MOVL
			//
			{1, 1, 4},// LOAD
			{1, 4, 1},// STORE
			//
			{4, 0, 0},// JMP
			{3, 0, 0},// FJMP
		};

		// Number of opcodes
		constexpr int count = sizeof(strings) / sizeof(char*);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Registers

	namespace Register {
		enum {
			BP = 0
		};
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Assembly function declarations
	
	struct AssemblyOptions {
		uint8_t flags = 0;
	};
	void Assemble(std::iostream& azm, std::ostream& eze, AssemblyOptions options);
	void Assemble(std::iostream& azm, std::ostream& eze, AssemblyOptions options, std::ostream& debug);
}