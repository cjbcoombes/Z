// Utilities for the vm
#pragma once
#include <exception>
static_assert(sizeof(void*) == 4, "No workaround for non-32-bit pointers");

namespace vm {

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Custom exceptions
	
	// Error during assembly
	class AssemblyError : public std::exception {
	public:

		// Error type enum
		static const enum ErrorType {
			UNKNOWN_ERROR
		};

		static constexpr const char* const test[] = { 
			"Unknown error" 
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
		static const enum ErrorType {
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
	// ## 
}