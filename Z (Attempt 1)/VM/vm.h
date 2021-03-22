// Utilities for the vm
#pragma once
#include <exception>
#include <iostream>
#include <fstream>
#include <limits>
static_assert(sizeof(void*) == 4, "No workaround for non-32-bit pointers");
#define VM_DEBUG

namespace vm {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Typedefs

	// Types and sizes of various items
	namespace types {
		typedef uint8_t register_t;
	}


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Custom exceptions
	
	// Error during assembly
	class AssemblyError : public std::exception {
	public:

		// Error type enum
		const enum ErrorType {
			UNKNOWN_ERROR,
			MAX_STR_SIZE_REACHED
		};

		static constexpr const char* const test[] = { 
			"Unknown error",
			"The maximum size of a contiguous string (no whitespace) has been reached"
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

			size = file.gcount();
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
	// ## Assembly function declarations
	
	struct AssemblyOptions {
		uint8_t flags;
	};
	void Assemble(std::iostream azm, std::ostream eze, AssemblyOptions options);
	void Assemble(std::iostream azm, std::ostream eze, AssemblyOptions options, std::ostream debug);
}