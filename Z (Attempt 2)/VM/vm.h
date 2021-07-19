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
		// Register ID: bute
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
				STRING_TOO_LONG
			};

			static constexpr const char* const errorStrings[] = {
				"String too long"
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


		int assemble(const char* const& assemblyPath, const char* const& outputPath, Flags& assemblyFlags);
		int assemble_(std::iostream& assemblyFile, std::iostream& outputFile, Flags& assemblyFlags, std::ostream& stream);

		types::reg_t parseRegister(char* const& str, const int& strlen);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Executor
	namespace executor {
		int exec(const char* const& path, Flags& execFlags);
		int exec_(std::iostream& file, Flags& execFlags);
	}
}