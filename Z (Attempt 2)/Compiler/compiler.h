#include "../utils.h"
#include <vector>
#include <list>

#include "ast.h"

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
		AND,
		OR,
		TRUE,
		FALSE,
		// Nums
		NUM_UNIDENTIFIED,
		NUM_INT,
		NUM_FLOAT
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
		"elif",
		"and",
		"or",
		"true",
		"false"
	};
	constexpr int numKeywords = ARR_LEN(keywords);

	struct NoDestructToken {
		NoDestructToken(const NoDestructToken&) = delete;
		NoDestructToken& operator=(const NoDestructToken&) = delete;
		NoDestructToken(NoDestructToken&&) = default;
		NoDestructToken& operator=(NoDestructToken&&) = default;

		TokenType type;
		bool hasStr;
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
	// AST
	
	namespace AST {
		enum class NodeType {
			NONE,
			TOKEN,
			PAREN_GROUP,
			SQUARE_GROUP,
			CURLY_GROUP,
			IDENTIFIER,
			INT,
			FLOAT,
			BOOL,
			CAST,
			BINOP
		};

		enum class ExprType {
			UNKNOWN,
			INT,
			FLOAT,
			BOOL
		};

		constexpr const char* const exprTypeNames[] = {
			"Unknown",
			"Int",
			"Float",
			"Bool"
		};

		enum class OpType {
			ADD,
			SUB,
			MULT,
			DIV
		};

		//

		struct ArithmeticBinopPattern {
			ExprType left;
			ExprType right;
			ExprType result;

			ArithmeticBinopPattern(ExprType leftIn, ExprType rightIn, ExprType resultIn) : left(leftIn), right(rightIn), result(resultIn) {}
		};

		const ArithmeticBinopPattern arithmeticBinopPatterns[] = {
			{ExprType::INT, ExprType::INT, ExprType::INT}
		};

		//

		struct Node {
			const NodeType type;
			bool isExpr;

			Node() : type(NodeType::NONE), isExpr(false) {}
			Node(AST::NodeType typeIn) : type(typeIn), isExpr(false) {}
			Node(AST::NodeType typeIn, bool isExprIn) : type(typeIn), isExpr(isExprIn) {}
			virtual ~Node() {}

			virtual void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "Node: " << static_cast<int>(type) << '\n';
			}
		};

		class NodeList : public std::list<Node*> {
		public:
			void print(std::ostream& stream, int indent) {
				for (auto ptr = begin(); ptr != end(); ptr++) {
					(*ptr)->print(stream, indent);
				}
			}

			~NodeList() {
				// Cleanup: delete nodes (nodes have destructors that delete their children)
				for (auto ptr = begin(); ptr != end(); ptr++) {
					delete (*ptr);
				}
			}
		};

		//

		struct NodeParenGroup : public Node {
			NodeList nodeList;

			NodeParenGroup() : Node(NodeType::PAREN_GROUP) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << '(' << '\n';
				nodeList.print(stream, indent + 1);
				stream << std::string(indent, '\t') << ')' << '\n';
			}
		};

		struct NodeSquareGroup : public Node {
			NodeList nodeList;

			NodeSquareGroup() : Node(NodeType::SQUARE_GROUP) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << '[' << '\n';
				nodeList.print(stream, indent + 1);
				stream << std::string(indent, '\t') << ']' << '\n';
			}
		};

		struct NodeCurlyGroup : public Node {
			NodeList nodeList;

			NodeCurlyGroup() : Node(NodeType::CURLY_GROUP) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << '{' << '\n';
				nodeList.print(stream, indent + 1);
				stream << std::string(indent, '\t') << '}' << '\n';
			}
		};

		//

		struct Expr : public Node {
			const ExprType evalType;

			Expr(NodeType typeIn, ExprType evalTypeIn) : Node(typeIn, true), evalType(evalTypeIn) {}
		};

		struct NodeToken : public Node {
			TokenList::iterator token;

			NodeToken(TokenList::iterator tokenIn) : Node(NodeType::TOKEN), token(tokenIn) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "Token: " << static_cast<int>(token->type) << '\n';
			}
		};

		//

		struct ExprInt : public Expr {
			int_t int_;

			ExprInt(int_t intIn) : Expr(NodeType::INT, ExprType::INT), int_(intIn) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "Int: " << int_ << '\n';
			}
		};

		//

		struct ExprFloat : public Expr {
			float_t float_;

			ExprFloat(float_t floatIn) : Expr(NodeType::FLOAT, ExprType::FLOAT), float_(floatIn) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "Float: " << float_ << '\n';
			}
		};

		//

		struct ExprBool : public Expr {
			bool_t bool_;

			ExprBool(bool_t boolIn) : Expr(NodeType::BOOL, ExprType::BOOL), bool_(boolIn) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "Bool: " << bool_ << '\n';
			}
		};

		//

		struct ExprIdentifier : public Expr {
			std::string* str;

			ExprIdentifier(std::string* strIn) : Expr(NodeType::IDENTIFIER, ExprType::UNKNOWN), str(strIn) {}
			~ExprIdentifier() {
				std::cout << "[String Deleted!]";
				delete str;
			}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "ID: " << (*str) << '\n';
			}
		};

		//

		struct ExprCast : public Expr {
			Expr* source;

			ExprCast(Expr* sourceIn, ExprType typeIn) : Expr(NodeType::CAST, typeIn), source(sourceIn) {}
			~ExprCast() {
				delete source;
			}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "Cast to type " << exprTypeNames[static_cast<int>(evalType)] << '\n';
				source->print(stream, indent + 1);
			}
		};

		struct ExprBinop : public Expr {
			Expr* left;
			Expr* right;
			OpType opType;

			ExprBinop(Expr* leftIn, Expr* rightIn, OpType opTypeIn, ExprType typeIn) : Expr(NodeType::BINOP, typeIn), left(leftIn), right(rightIn), opType(opTypeIn) {}
			~ExprBinop() {
				delete left;
				delete right;
			}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "Binop with result type " << exprTypeNames[static_cast<int>(evalType)] << static_cast<int>(opType) << '\n';
				left->print(stream, indent + 1);
				stream << std::string(indent + 1, '\t') << '\n';
				left->print(stream, indent + 1);
			}
		};

		struct Tree {

		};
	}

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
			STRING_TOO_LONG,
			INVALID_NUMBER,
			INVALID_CLOSING_PAREN,
			INVALID_CLOSING_SQUARE,
			INVALID_CLOSING_CURLY,
			BINOP_MISSING_EXPRESSION,
		};

		static constexpr const char* const errorStrings[] = {
			"String too long",
			"Invalid number",
			"Invalid closing parenthesis",
			"Invalid closing square bracket",
			"Invalid closing curly bracket",
			"Binop is missing an expression on one or both sides"
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
	int constructAST(AST::Tree& tree, TokenList& tokenList, std::ostream& stream);
	int condenseAST(AST::NodeList& list, AST::NodeList& subList, AST::NodeList::iterator start, AST::NodeType type);
}