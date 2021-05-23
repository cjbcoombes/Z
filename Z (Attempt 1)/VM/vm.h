// Utilities for the vm
#pragma once
#include <exception>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>

#define VM_DEBUG

#define TO_CHAR(x) \
	reinterpret_cast<char*>(&(x))

#define STRM_DEFAULT \
	std::right << std::setfill(' ') << std::nouppercase << std::dec

#define STRM_HEX \
	std::right << std::setfill('0') << std::uppercase << std::hex

namespace vm {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Typedefs

	// Types and sizes of various items
	namespace types {
		// Size of a word
		typedef uint32_t word_t;
		// Size of a byte
		typedef uint8_t byte_t;
		// Size of a short
		typedef uint16_t short_t;

		// Register id
		typedef uint8_t register_t;
		// Opcode id
		typedef uint8_t opcode_t;

		// Memory offset (short)
		typedef int16_t offset_t;
		// Memory address (word)
		typedef int32_t address_t;
		// Char (byte)
		typedef int8_t char_t;
		// Int (word)
		typedef int32_t int_t;

		union Value {
			// For storing objects of various arbitrary sizes
			word_t word;
			byte_t byte;
			short_t short_;

			int_t i;
			char_t c;
			address_t addr;
		};

		static_assert(sizeof(std::intptr_t) == sizeof(word_t), "No workaround for non-word-size (32-bit) pointers");
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
			UNBALANCED_PARENS,
			UNDEFINED_VAR,
			INVALID_GLOBAL,
			INVALID_OPCODE,
			INVALID_REGISTER,
			INVALID_WORD,
			INVALID_BYTE,
			INVALID_SHORT
		};

		static constexpr const char* const strings[] = {
			"Unknown error",
			"The maximum size of a contiguous string (no whitespace) has been reached",
			"Unbalanced parentheses",
			"Undefined variable",
			"Globals must be defined at the top of the program",
			"Invalid opcode",
			"Invalid register",
			"Invalid word",
			"Invalid byte",
			"Invalid short"
		};

		const ErrorType type;
		std::string extra;
		bool isExtra;
	#pragma warning( push )
	#pragma warning( disable: 26812 ) // Prefer 'enum class' over unscoped 'enum'
		AssemblyError(ErrorType eType) : type(eType), isExtra(false) {}
		AssemblyError(ErrorType eType, char* const& str) : type(eType), extra(str), isExtra(true) {}
	#pragma warning( pop )

		virtual const char* what() {
			if (isExtra) {
				extra.insert(0, " : ");
				extra.insert(0, strings[type]);
				return extra.c_str();
			}
			return strings[type];
		}
	};

	// Error during execution
	class ExecError : public std::exception {
	public:

		// Error type enum
		const enum ErrorType {
			UNKNOWN_ERROR,
			FILE_OVERREAD,
			UNKNOWN_OPCODE
		};

		static constexpr const char* const test[] = {
			"Unknown error",
			"Read past the end of the file",
			"Unknown opcode"
		};

		const ErrorType type;
	#pragma warning( push )
	#pragma warning( disable: 26812 ) // Prefer 'enum class' over unscoped 'enum'
		ExecError(ErrorType eType) : type(eType) {}
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
		static constexpr unsigned int MAX_BUFFER_SIZE = UINT_MAX;

		char* start;
		char* ptr;
		char* end;
		unsigned int size;

		FileBuffer(std::iostream& file) {
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

		~FileBuffer() {
			delete[] start;
		}

		template<typename T>
		void read(T* val) {
			(*val) = *reinterpret_cast<T*>(ptr);
			ptr += sizeof(T);
		}

		template<typename T>
		void readCheck(T* val) {
			if (ptr + sizeof(T) > end)
				throw vm::ExecError(vm::ExecError::FILE_OVERREAD);
			(*val) = *reinterpret_cast<T*>(ptr);
			ptr += sizeof(T);
		}

		void checkNum(int n) {
			if (ptr + n > end)
				throw vm::ExecError(vm::ExecError::FILE_OVERREAD);
		}

		bool atEnd() {
			return ptr >= end;
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
			STRPRNT,
			RPRNT,
			LNPRNT,
			//
			MOV,
			MOVW,
			MOVB,
			//
			LOADW,
			STOREW,
			LOADB,
			STOREB,
			//
			JMP,
			JMPZ,
			JMPNZ,
			//
			ICMP,
			IADD,
			//
			// 
			// 
			VALID_OPCODE_BREAK,
			// The following are on the list as codes in assembly, but are not executable
			LOADGW,
			STOREGW,
			LOADGB,
			STOREGB,
			STRPRNTG,
			//
			GLOBAL_OPCODE_BREAK,
			// The following are for adding global variables to the header of the executable
			GLOBALW,
			GLOBALB,
			GLOBALSTR
		};

		// Strings for opcodes
		constexpr const char* const strings[] = {
			"nop",
			//
			"halt",
			//
			"strprnt",
			"rprnt",
			"lnprnt",
			//
			"mov",
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
			//
			"icmp",
			"iadd",
			//
			// 
			// 
			"",
			//
			"loadgw",
			"storegw",
			"loadgb",
			"storegb",
			"strprntg",
			//
			"",
			//
			"globalw",
			"globalb",
			"globalstr"
		};

		constexpr int MAX_ARGS = 3;
		namespace ArgType {
			enum Type {
				ARG_NONE,	// 0
				ARG_REG,	// 1
				ARG_WORD,	// 2
				ARG_BYTE,	// 3
				ARG_SHORT,	// 4
				ARG_LABEL,	// 5
				ARG_VAR,	// 6
				ARG_STR,	// 7
			};
		}
		constexpr int args[][MAX_ARGS] = {
			{0, 0, 0},// NOP
			//
			{0, 0, 0},// HALT
			//
			{1, 4, 0},// STRPRNT
			{1, 0, 0},// RPRNT
			{0, 0, 0},// LNPRNT
			//
			{1, 1, 0},// MOV
			{1, 2, 0},// MOVW
			{1, 3, 0},// MOVB
			//
			{1, 1, 4},// LOADW
			{1, 4, 1},// STOREW
			{1, 1, 4},// LOADB
			{1, 4, 1},// STOREB
			//
			{5, 0, 0},// JMP
			{5, 0, 0},// JMPZ
			{5, 0, 0},// JMPNZ
			//
			{1, 0, 0},// ICMP
			{1, 1, 1},// IADD
			//
			// 
			// 
			{0, 0, 0},// VALID_OPCODE_BREAK
			//
			{1, 6, 0},// LOADGW
			{6, 1, 0},// STOREGW
			{1, 6, 0},// LOADGB
			{6, 1, 0},// STOREGB
			{6, 0, 0},// STRPRNTG
			//
			{0, 0, 0},// GLOBAL_OPCODE_BREAK
			//
			{6, 2, 0},// GLOBALW
			{6, 3, 0},// GLOBALB
			{6, 7, 0} // GLOBALSTR
		};

		// Number of opcodes
		constexpr int count = sizeof(strings) / sizeof(char*);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Registers

	namespace Register {
		constexpr int GP_REGISTER_OFFSET = 2;
		constexpr int GP_REGISTER_COUNT = 30;
		enum {
			BP = 0,
			GP = 1
		};
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Registers
	namespace Flag {
		typedef uint8_t flag_t;
		enum {
			ZERO = 1,
			NEG = 2
		};

		template <typename T>
		void set(flag_t& flags, T num) {
			flags = 0;
			if (num < 0)
				flags |= NEG;
			else if (num == 0)
				flags |= ZERO;
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Stack

	// Container for stack with automatic destructor dynamic deallocation
	class Stack {
	public:
		static constexpr unsigned int STACK_SIZE = 2048;

		char* top;// Actually bottom
		char* bottom;// Actually top

		Stack() {
			top = new char[STACK_SIZE];
			bottom = top + STACK_SIZE;
		}

		~Stack() {
			delete[] top;
		}
	};

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Assembly function declarations

	struct AssemblyOptions {};
	void Assemble(std::iostream& asm_, std::ostream& exe, AssemblyOptions options);
	void Assemble(std::iostream& asm_, std::ostream& exe, AssemblyOptions options, std::ostream& debug);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Exec function declarations

	struct ExecOptions {
		static enum {
			PRINT_OPCODE = 1,
			PROFILE = 2
		};
		uint8_t flags;

		ExecOptions() : flags(0) {}
		ExecOptions(uint8_t inFlags) : flags(inFlags) {}
	};
	void Exec(std::iostream& exe, std::ostream& output, ExecOptions options);
	void Exec(std::iostream& exe, std::ostream& output, ExecOptions options, std::ostream& debug);
}