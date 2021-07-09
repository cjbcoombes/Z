#pragma once

#include <iostream>
#include <iomanip>
#include <vector>

#define COMP_DEBUG

#define STRM_DEFAULT \
	std::right << std::setfill(' ') << std::nouppercase << std::dec

#define STRM_HEX \
	std::right << std::setfill('0') << std::uppercase << std::hex

namespace compile {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Expressions

	enum class ExprType {
		BINARY
	};

	struct Expr {
		const ExprType type;
		Expr(const ExprType typeIn) : type(typeIn) {}
	};

	/*struct BinaryExpr : Expr {
		Expr* child1;
		Expr* child2;

		BinaryExpr() : Expr(ExprType::BINARY), child1(nullptr), child2(nullptr) {}
	};*/

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Tokens 
	namespace TokenType {
		enum Type {
			IDENTIFIER,
			STRING,
			NUMBER,
			//
			LEFT_PAREN,
			RIGHT_PAREN,
			LEFT_BRACKET,
			RIGHT_BRACKET,
			LEFT_CURLY,
			RIGHT_CURLY,
			//
			TILDE,			// ~
			BACKTICK,		// `
			NOT,			// !
			AT,				// @
			HASH,			// #
			DOLLAR,			// $
			PCT,			// %
			CARET,			// ^
			AMP,			// &
			AST,			// *
			MINUS,			// -
			PLUS,			// +
			EQ,				// =
			//
			PIPE,			// |
			BACKSLASH,		// Can't type this one...
			COLON,			// :
			SEMICOLON,		// ;
			DBL_QUOTE,		// "
			QUOTE,			// '
			LT,				// <
			COMMA,			// ,
			GT,				// >
			PERIOD,			// .
			Q_MARK,			// ?
			SLASH,			// /
			//
			LT_EQ,			// <=
			GT_EQ,			// >=
			EQ_EQ,			// ==
			NOT_EQ,			// !=
			//
			IF,
			ELSE,
			ELIF,
			WHILE,
			FOR,
			RETURN,
			AND,
			OR,
			TRUE,
			FALSE,
			//
			INT
		};

		constexpr char const chars[] = {
			' ',
			//
			'(',
			')',
			'[',
			']',
			'{',
			'}',
			//
		};
	}

	struct Token : Expr {
		TokenType::Type type;
		std::string value;
	};

	typedef std::vector<Token> TokenList;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ## Tokenizer Function Declarations

	void Tokenize(std::iostream& file, TokenList& list);
	void Tokenize(std::iostream& file, TokenList& list, std::ostream& debug);
}