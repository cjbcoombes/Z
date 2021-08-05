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
				INVALID_SHORT_PARSE
			};

			static constexpr const char* const errorStrings[] = {
				"String too long",
				"Invalid opcode during parsing",
				"Invalid register during parsing",
				"Invalid word during parsing",
				"Invalid byte during parsing",
				"Invalid short during parsing",
			};

			const ErrorType eType;
			const int line;
			const int column;

			AssemblerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn) : eType(eTypeIn), line(lineIn), column(columnIn) {}


			virtual const char* what() const {
				return errorStrings[eType];
			}
		};

		constexpr int MAX_STR_SIZE = 256;

		namespace format {
			constexpr int STACK_SIZE_LOCATION = 0;
			constexpr int FIRST_INSTR_ADDR_LOCATION = 4;
			constexpr int GLOBAL_TABLE_LOCATION = 8;
		}

		int assemble(const char* const& assemblyPath, const char* const& outputPath, Flags& assemblyFlags);
		int assemble_(std::iostream& assemblyFile, std::iostream& outputFile, Flags& assemblyFlags, std::ostream& stream);

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Parsing

		types::reg_t parseRegister(char* const& str, const int& strlen, const int& line, const int& column);
		template<typename T, AssemblerException::ErrorType eType>
		T parseNumber(char* const& str, int strlen, const int& line, const int& column);
		
		// Declare for number types
		template types::reg_t parseNumber<types::reg_t, AssemblerException::INVALID_REG_PARSE>(char* const&, int, const int&, const int&);
		template types::word_t parseNumber<types::word_t, AssemblerException::INVALID_WORD_PARSE>(char* const&, int, const int&, const int&);
		template types::byte_t parseNumber<types::byte_t, AssemblerException::INVALID_WORD_PARSE>(char* const&, int, const int&, const int&);
		template types::short_t parseNumber<types::short_t, AssemblerException::INVALID_WORD_PARSE>(char* const&, int, const int&, const int&);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Executor
	namespace executor {
		int exec(const char* const& path, Flags& execFlags);
		int exec_(std::iostream& file, Flags& execFlags);
	}
}