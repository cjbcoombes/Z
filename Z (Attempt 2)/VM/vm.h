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
		// 16-bit short: address offsets
		typedef int16_t short_t;

		// Opcode ID: byte
		typedef uint8_t opcode_t;
		// Register ID: byte
		typedef uint8_t reg_t;

		union Value {
			word_t word;
			byte_t byte;
			short_t short_;
		};

		static_assert(sizeof(std::intptr_t) == sizeof(word_t), "No workaround for non-word-size (32-bit) pointers");
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constants?
	constexpr const types::word_t wordPlaceholder = 0xbcbcbcbc;
	constexpr const types::word_t wordErr = 0xecececec;

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

		namespace format {
			constexpr int FIRST_INSTR_ADDR_LOCATION = 0;
			constexpr int GLOBAL_TABLE_LOCATION = 4;
		}

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
				TEST
			};

			static constexpr const char* const errorStrings[] = {
				"Test"
			};

			const ErrorType eType;
			std::string extra;

			ExecutorException(const ErrorType& eTypeIn) : eType(eTypeIn), extra("") {}
			ExecutorException(const ErrorType& eTypeIn, char* const& extraIn) : eType(eTypeIn), extra(extraIn) {}
			ExecutorException(const ErrorType& eTypeIn, const char* const& extraIn) : eType(eTypeIn), extra(extraIn) {}
			ExecutorException(const ErrorType& eTypeIn, const std::string& extraIn) : eType(eTypeIn), extra(extraIn) {}


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

		int exec(const char* const& path, ExecutorSettings& execSettings);
		int exec_(std::iostream& file, ExecutorSettings& execSettings, std::ostream& stream);
	}
}