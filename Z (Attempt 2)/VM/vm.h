#pragma once

#include "..\utils.h"
#include "opcode.h"
#include "register.h"

namespace vm {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Flags
	constexpr int FLAG_DEBUG = 1;
	constexpr int FLAG_PROFILE = 2;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Types
	namespace types {
		// 32-bit word: addresses, integers
		typedef int32_t word_t;
		// 8-bit byte: register IDs, opcode IDs, chars
		typedef int8_t byte_t;
		// 16-bit short: 
		typedef int16_t short_t;

		// Opcode ID: byte
		typedef uint8_t opcode_t;
		// Register ID: byte
		typedef uint8_t reg_t;

		// Integer: word
		typedef int32_t int_t;
		// Char: byte
		typedef int8_t char_t;
		// Bool: byte
		typedef int8_t bool_t;

		static_assert(sizeof(std::intptr_t) == sizeof(word_t), "No workaround for non-word-size (32-bit) pointers");
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constants?
	constexpr types::word_t wordPlaceholder = 0xbcbcbcbc;
	constexpr types::word_t wordErr = 0xecececec;
	constexpr char charFiller = opcode::HALT;

	namespace format {
		constexpr int FIRST_INSTR_ADDR_LOCATION = 0;
		constexpr int GLOBAL_TABLE_LOCATION = 4;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Opcodes: Included in separate header file


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Registers: Included in separate header file


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Assembler

	namespace assembler {
		class AssemblerException : public std::exception {
		public:
			enum ErrorType {
				STRING_TOO_LONG,
				INVALID_OPCODE_PARSE,
				INVALID_REG_PARSE,
				INVALID_WORD_PARSE,
				INVALID_BYTE_PARSE,
				INVALID_SHORT_PARSE,
				INVALID_VAR_PARSE,
				UNDEFINED_LABEL,
				UNDEFINED_VAR,
				MISPLACED_GLOBAL
			};

			static constexpr const char* const errorStrings[] = {
				"String too long",
				"Invalid opcode during parsing",
				"Invalid register during parsing",
				"Invalid word during parsing",
				"Invalid byte during parsing",
				"Invalid short during parsing",
				"Invalid global variable during parsing",
				"Undefined label",
				"Undefined global variable",
				"Cannot attempt to set global variable after normal program opcodes"
			};

			const ErrorType eType;
			const int line;
			const int column;
			std::string extra;

			AssemblerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn) : eType(eTypeIn), line(lineIn), column(columnIn), extra("") {}
			AssemblerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn, char* const& extraIn) : eType(eTypeIn), line(lineIn), column(columnIn), extra(extraIn) {}
			AssemblerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn, const char* const& extraIn) : eType(eTypeIn), line(lineIn), column(columnIn), extra(extraIn) {}
			AssemblerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn, const std::string& extraIn) : eType(eTypeIn), line(lineIn), column(columnIn), extra(extraIn) {}


			virtual const char* what() {
				if (extra.length() == 0) {
					return errorStrings[eType];
				} else {
					extra.insert(0, " : ");
					extra.insert(0, errorStrings[eType]);
					return extra.c_str();
				}
			}
		};

		struct AssemblerSettings {
			Flags flags;
		};

		constexpr int MAX_STR_SIZE = 256;

		int assemble(const char* const& assemblyPath, const char* const& outputPath, AssemblerSettings& assemblerSettings);
		int assemble_(std::iostream& assemblyFile, std::iostream& outputFile, AssemblerSettings& assemblerSettings, std::ostream& stream);

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Parsing

		types::reg_t parseRegister(char* const& str, const int& strlen, const int& line, const int& column);
		template<typename T, AssemblerException::ErrorType eType>
		T parseNumber(const char* str, int strlen, const int& line, const int& column);
		
		// Declare for number types
		template types::reg_t parseNumber<types::reg_t, AssemblerException::INVALID_REG_PARSE>(const char*, int, const int&, const int&);
		template types::word_t parseNumber<types::word_t, AssemblerException::INVALID_WORD_PARSE>(const char*, int, const int&, const int&);
		template types::byte_t parseNumber<types::byte_t, AssemblerException::INVALID_WORD_PARSE>(const char*, int, const int&, const int&);
		template types::short_t parseNumber<types::short_t, AssemblerException::INVALID_WORD_PARSE>(const char*, int, const int&, const int&);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Executor
	namespace executor {
		class ExecutorException : public std::exception {
		public:
			enum ErrorType {
				UNKNOWN_OPCODE,
				DIVIDE_BY_ZERO
			};

			static constexpr const char* const errorStrings[] = {
				"Unknown opcode",
				"Division (or modulo) by zero"
			};

			const ErrorType eType;
			const int loc;
			std::string extra;

			ExecutorException(const ErrorType& eTypeIn, const int& locIn) : eType(eTypeIn), loc(locIn), extra("") {}
			ExecutorException(const ErrorType& eTypeIn, const int& locIn, char* const& extraIn) : eType(eTypeIn), loc(locIn), extra(extraIn) {}
			ExecutorException(const ErrorType& eTypeIn, const int& locIn, const char* const& extraIn) : eType(eTypeIn), loc(locIn), extra(extraIn) {}
			ExecutorException(const ErrorType& eTypeIn, const int& locIn, const std::string& extraIn) : eType(eTypeIn), loc(locIn), extra(extraIn) {}


			virtual const char* what() {
				if (extra.length() == 0) {
					return errorStrings[eType];
				} else {
					extra.insert(0, " : ");
					extra.insert(0, errorStrings[eType]);
					return extra.c_str();
				}
			}
		};

		struct ExecutorSettings {
			Flags flags;
			unsigned int stackSize;

			ExecutorSettings() : stackSize(0x1000) {}
		};

		union Value {
			types::word_t word;
			types::byte_t byte;
			types::short_t short_;

			types::int_t int_;
			types::char_t char_;
			types::bool_t bool_;

			Value() : word(0) {}
		};

		class Program {
			static constexpr int FILLER_SIZE = 24;

		public:
			char* start;
			char* ip;
			char* end;

			Program(std::iostream& program) {
				// https://stackoverflow.com/questions/22984956/tellg-function-give-wrong-size-of-file
				program.seekg(0, std::ios::beg);
				program.ignore(std::numeric_limits<std::streamsize>::max());
				std::streamsize length = program.gcount();

				start = new char[length + FILLER_SIZE];
				ip = start;
				end = start + length;

				program.clear();
				program.seekg(0, std::ios::beg);
				program.read(start, length);

				std::fill(start + length, start + length + FILLER_SIZE, charFiller);
			}

			~Program() {
				delete[] start;
			}

			void goto_(vm::types::word_t loc) {
				ip = start + loc;
			}

			template<typename T>
			void read(T* val) {
				(*val) = *reinterpret_cast<T*>(ip);
				ip += sizeof(T);
			}
		};

		class Stack {
		public:
			char* start;
			char* end;

			Stack(const int& size) {
				start = new char[size];
				end = start + size;
			}

			~Stack() {
				delete[] start;
			}
		};

		int exec(const char* const& path, ExecutorSettings& execSettings);
		int exec_(std::iostream& file, ExecutorSettings& execSettings, std::ostream& stream);
	}
}