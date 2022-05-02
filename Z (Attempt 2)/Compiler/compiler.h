#include "../utils.h"
#include <vector>

namespace compiler {
	using namespace types;

	constexpr int MAX_STR_SIZE = 1024;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Tokens

	enum class TokenType {
		IDENTIFIER,
		PRIMITIVE_TYPE,
		STRING,
		CHAR,
		// Token1s
		TILDE,
		BTICK,
		EXPT,
		AT,
		HASH,
		DOLLAR,
		PCT,
		CARET,
		AMP,
		STAR,
		USCORE,
		DASH,
		PLUS,
		EQUALS,
		PIPE,
		BSLASH,
		COLON,
		SEMICOLON,
		DBLQUOTE,
		QUOTE,
		COMMA,
		PERIOD,
		QMARK,
		SLASH,
		LEFT_PAREN,
		RIGHT_PAREN,
		LEFT_SQUARE,
		RIGHT_SQUARE,
		LEFT_CURLY,
		RIGHT_CURLY,
		LEFT_ANGLE,
		RIGHT_ANGLE,
		// Token2s
		PLUS_EQUALS,
		DASH_EQUALS,
		STAR_EQUALS,
		SLASH_EQUALS,
		PCT_EQUALS,
		EQ_EQUALS,
		LEFT_ANGLE_EQUALS,
		RIGHT_ANGLE_EQUALS,
		PLUS_PLUS,
		DASH_DASH,
		SLASH_SLASH,
		SLASH_STAR,
		STAR_SLASH,
		// Other stuff
		RETURN,
		WHILE,
		FOR,
		IF,
		ELSE,
		ELIF,
		// Nums
		NUM_UNIDENTIFIED
	};

	// token1 means a single-char token
	constexpr int firstToken1 = static_cast<int>(TokenType::TILDE);
	constexpr char token1s[] = {
		'~', '`',  '!', '@', '#', '$', '%',  '^', '&', '*', '_', '-', '+', '=',
		'|', '\\', ':', ';', '"', '\'', ',', '.', '?', '/',
		'(', ')', '[', ']', '{', '}', '<', '>'
	};
	constexpr int numToken1s = ARR_LEN(token1s);

	constexpr int firstToken2 = static_cast<int>(TokenType::PLUS_EQUALS);
	constexpr std::pair<TokenType, char> token2s[] = {
		{ TokenType::PLUS, '=' },
		{ TokenType::DASH, '=' },
		{ TokenType::STAR, '=' },
		{ TokenType::SLASH, '=' },
		{ TokenType::PCT, '=' },
		{ TokenType::EQUALS, '=' },
		{ TokenType::LEFT_ANGLE, '=' },
		{ TokenType::RIGHT_ANGLE, '=' },
		{ TokenType::PLUS, '+' },
		{ TokenType::DASH, '-' },
		{ TokenType::SLASH, '/' },
		{ TokenType::SLASH, '*' },
		{ TokenType::STAR, '/' }
	};
	constexpr int numToken2s = ARR_LEN(token2s);

	enum class PrimitiveType {
		INT,
		FLOAT,
		CHAR,
		BOOL
	};
	constexpr const char* const primTypes[] = {
		"int",
		"float",
		"bool",
		"char"
	};
	constexpr int numPrimTypes = ARR_LEN(primTypes);

	constexpr int firstKeyword = static_cast<int>(TokenType::RETURN);
	constexpr const char* const keywords[] = {
		"return",
		"while",
		"for",
		"if",
		"else",
		"elif"
	};
	constexpr int numKeywords = ARR_LEN(keywords);

	struct NoDestructToken {
		NoDestructToken(const NoDestructToken&) = delete;
		NoDestructToken& operator=(const NoDestructToken&) = delete;
		NoDestructToken(NoDestructToken&&) = default;
		NoDestructToken& operator=(NoDestructToken&&) = default;

		const TokenType type;
		const bool hasStr;
		const int line;
		const int column;

		union {
			word_t word;
			float_t float_;
			int_t int_;

			byte_t byte;
			char_t char_;
			bool_t bool_;

			PrimitiveType primType;

			std::string* str;
		};

		NoDestructToken(TokenType typeIn, int lineIn, int columnIn) : type(typeIn), hasStr(false), str(nullptr), line(lineIn), column(columnIn) {}
		NoDestructToken(TokenType typeIn, int lineIn, int columnIn, std::string* strIn) : type(typeIn), hasStr(true), str(strIn), line(lineIn), column(columnIn) {}
		NoDestructToken(TokenType typeIn, int lineIn, int columnIn, PrimitiveType primTypeIn) : type(typeIn), hasStr(true), primType(primTypeIn), line(lineIn), column(columnIn) {}
	};
	
	class TokenList : public std::vector<NoDestructToken> {
	public:
		~TokenList() {
			for (auto ptr = begin(); ptr < end(); ptr++) {
				if (ptr->hasStr) delete ptr->str;
			}
		}
	};

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Settings

	struct CompilerSettings {
		Flags flags;
	};

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Exceptions

	class CompilerException : public std::exception {
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
		std::string extra;

		CompilerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn) : eType(eTypeIn), line(lineIn), column(columnIn), extra("") {}
		CompilerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn, char* const& extraIn) : eType(eTypeIn), line(lineIn), column(columnIn), extra(extraIn) {}
		CompilerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn, const char* const& extraIn) : eType(eTypeIn), line(lineIn), column(columnIn), extra(extraIn) {}
		CompilerException(const ErrorType& eTypeIn, const int& lineIn, const int& columnIn, const std::string& extraIn) : eType(eTypeIn), line(lineIn), column(columnIn), extra(extraIn) {}


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

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Functions

	int compile(const char* const& inputPath, const char* const& outputPath, CompilerSettings& settings);
	int compile_(std::iostream& inputFile, std::iostream& outputFile, CompilerSettings& settings, std::ostream& stream);
	int tokenize(TokenList& tokenList, std::iostream& file, std::ostream& stream);
	int parseNumber(NoDestructToken& token);
}