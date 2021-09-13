#include "vm.h"

namespace vm {
	namespace assembler {
		class AssemblerException : public std::exception {
		public:
			enum ErrorType {
				STRING_TOO_LONG,
				INVALID_OPCODE_PARSE,
				INVALID_WORD_REG_PARSE,
				INVALID_BYTE_REG_PARSE,
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
				"Invalid word register during parsing",
				"Invalid byte register during parsing",
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

		types::reg_t parseWordRegister(char* const& str, const int& strlen, const int& line, const int& column);
		types::reg_t parseByteRegister(char* const& str, const int& strlen, const int& line, const int& column);
		template<typename T, AssemblerException::ErrorType eType>
		T parseNumber(const char* str, int strlen, const int& line, const int& column);

		// Declare for number types
		template types::reg_t parseNumber<types::reg_t, AssemblerException::INVALID_WORD_REG_PARSE>(const char*, int, const int&, const int&);
		template types::reg_t parseNumber<types::reg_t, AssemblerException::INVALID_BYTE_REG_PARSE>(const char*, int, const int&, const int&);
		template types::word_t parseNumber<types::word_t, AssemblerException::INVALID_WORD_PARSE>(const char*, int, const int&, const int&);
		template types::byte_t parseNumber<types::byte_t, AssemblerException::INVALID_BYTE_PARSE>(const char*, int, const int&, const int&);
	}
}