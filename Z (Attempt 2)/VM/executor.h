#include "vm.h"

namespace vm {
	namespace executor {
		class ExecutorException : public std::exception {
		public:
			enum ErrorType {
				UNKNOWN_OPCODE,
				DIVIDE_BY_ZERO,
				BAD_ALLOC
			};

			static constexpr const char* const errorStrings[] = {
				"Unknown opcode",
				"Division (or modulo) by zero",
				"Dynamic memory allocation error"
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

			void goto_(types::word_t loc) {
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
		int exec_(std::iostream& file, ExecutorSettings& execSettings, std::ostream& stream, std::istream& streamIn);
	}
}