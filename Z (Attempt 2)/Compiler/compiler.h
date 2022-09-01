#include "../utils.h"
#include <vector>
#include <list>

#include "ast.h"

namespace compiler {
	using namespace types;

	// Maximum number of characters in a string to be parsed
	constexpr int MAX_STR_SIZE = 1024;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Tokens

	// An enum for the type of a token
	enum class TokenType {
		IDENTIFIER,
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
		INT,
		FLOAT,
		BOOL,
		CHAR_,
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

	// Token1s are length-1 tokens
	constexpr int firstToken1 = static_cast<int>(TokenType::TILDE);
	constexpr char token1s[] = {
		'~', '`',  '!', '@', '#', '$', '%',  '^', '&', '*', '_', '-', '+', '=',
		'|', '\\', ':', ';', '"', '\'', ',', '.', '?', '/',
		'(', ')', '[', ']', '{', '}', '<', '>'
	};
	constexpr int numToken1s = ARR_LEN(token1s);

	// Token2s are length-2 tokens
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

	// Keywords are full strings that are matched exactly
	constexpr int firstKeyword = static_cast<int>(TokenType::INT);
	constexpr const char* const keywords[] = {
		"int",
		"float",
		"bool",
		"char",
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

	// Struct that holds information about a token
	/*	It's called "NoDestruct" because it shouldn't ever be deleted on its own.
		Due to some issues with destructors and constructors being called when a vector
		rearranges its memory, these should only be created as part of a TokenList and
		should only be destroyed by the TokenList's destructor method.
	*/
	struct NoDestructToken {
		NoDestructToken(const NoDestructToken&) = delete;
		NoDestructToken& operator=(const NoDestructToken&) = delete;
		NoDestructToken(NoDestructToken&&) = default;
		NoDestructToken& operator=(NoDestructToken&&) = default;

		TokenType type;
		bool hasStr;
		// The line and column are stored for printing error messages
		const int line;
		const int column;

		// Holds the tokens' data, whatever that may be
		union {
			word_t word;
			float_t float_;
			int_t int_;

			byte_t byte;
			char_t char_;
			bool_t bool_;

			std::string* str;
		};

		NoDestructToken(TokenType typeIn, int lineIn, int columnIn) : type(typeIn), hasStr(false), str(nullptr), line(lineIn), column(columnIn) {}
		NoDestructToken(TokenType typeIn, int lineIn, int columnIn, std::string* strIn) : type(typeIn), hasStr(true), str(strIn), line(lineIn), column(columnIn) {}
	};

	// A vector of NoDestructTokens with a custom destructor to manage the string pointers
	class TokenList : public std::vector<NoDestructToken> {
	public:
		// Deletes its tokens' strings, if they exist, bnfore the tokens themselves are destroyed
		~TokenList() {
			for (auto ptr = begin(); ptr < end(); ptr++) {
				if (ptr->hasStr) delete ptr->str;
			}
		}
	};

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// AST

	// Abstract Syntax Tree
	namespace AST {
		// Types of AST nodes
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
			CHAR,
			CAST,
			BINOP
		};

		// Types that an expression can evaluate to (OLD)
		enum class PrimType {
			UNKNOWN,
			INT,
			FLOAT,
			BOOL,
			CHAR
		};

		bool isWord(PrimType type);

		// Names of expression types (for printing to console)
		constexpr const char* const primTypeNames[] = {
			"Unknown",
			"Int",
			"Float",
			"Bool",
			"Char"
		};


		// How expressions keep track of what type they'll evaluate to
		struct EvalType {
			int name; // Name is an identifier that's been converted to a number id rather than a string
			int primType;

			EvalType() : name(-1), primType(-1) {}
			EvalType(bool primTypeIn) : name(-1), primType(primTypeIn) {}
			EvalType(PrimType primTypeIn) : name(-1) {
				primType = static_cast<int>(primTypeIn);
			}
			EvalType(int nameIn) : name(nameIn), primType(-1) {}
			EvalType(int nameIn, bool primTypeIn) : name(nameIn), primType(primTypeIn) {}
			EvalType(int nameIn, PrimType primTypeIn) : name(nameIn) {
				primType = static_cast<int>(primTypeIn);
			}
			virtual ~EvalType() {}

			const char* printName() const {
				return primType > -1 ? primTypeNames[primType] : "Comp Type";
			}
			PrimType getPrim() const {
				return primType > -1 ? static_cast<PrimType>(primType) : PrimType::UNKNOWN;
			}
			bool isPrim() const {
				return primType > -1;
			}
		};

		struct CompoundType : EvalType {
			EvalType* returnType;
			EvalType(*memberTypes)[];

			CompoundType() : EvalType(), returnType(nullptr), memberTypes(nullptr) {}
			CompoundType(int nameIn) : EvalType(nameIn), returnType(nullptr), memberTypes(nullptr) {}
			~CompoundType() {
				if (returnType != nullptr) delete returnType;
				if (memberTypes != nullptr) delete memberTypes;
			}
		};

		// Types of operations
		enum class OpType {
			ADD,
			SUB,
			MULT,
			DIV
		};

		// Names of operation types (for printing to console)
		constexpr const char* const opTypeNames[] = {
			"Add",
			"Sub",
			"Mult",
			"Div"
		};

		//

		// A pattern for the possible type patterns for a binop, and their implicit type casts
		struct ArithmeticBinopPattern {
			PrimType resultType;
			PrimType aType;
			PrimType bType;

			ArithmeticBinopPattern(PrimType r, PrimType a, PrimType b) : resultType(r), aType(a), bType(b) {}
			ArithmeticBinopPattern(PrimType a, PrimType b) : resultType(a), aType(a), bType(b) {}
			ArithmeticBinopPattern(PrimType a) : resultType(a), aType(a), bType(a) {}
		};

		// The valid patterns
		const ArithmeticBinopPattern arithmeticBinopPatterns[] = {
			{PrimType::INT},
			{PrimType::FLOAT},
			{PrimType::CHAR},
			{PrimType::FLOAT, PrimType::INT},
			{PrimType::FLOAT, PrimType::CHAR},
			{PrimType::FLOAT, PrimType::BOOL},
			{PrimType::INT, PrimType::CHAR},
			{PrimType::INT, PrimType::BOOL},
			{PrimType::CHAR, PrimType::BOOL},
			{PrimType::INT, PrimType::BOOL, PrimType::BOOL}
		};

		//

		// Possible identifier types
		enum class IdentifierType {
			VAR,
			FUNC,
			TYPE
			// NAMESPACE? CLASS/STRUCT?
		};

		// Data that is stored about an identifier (wow!)
		struct IdentifierData {
			std::string* str;
			IdentifierType type;
		};

		//

		// Generic AST node
		struct Node {
			const NodeType type;
			// Used to check if it evaluates to an expression
			bool isExpr;
			int line;
			int column;

			Node(int lineIn, int columnIn) : type(NodeType::NONE), isExpr(false), line(lineIn), column(columnIn) {}
			Node(AST::NodeType typeIn, int lineIn, int columnIn) : type(typeIn), isExpr(false), line(lineIn), column(columnIn) {}
			Node(AST::NodeType typeIn, bool isExprIn, int lineIn, int columnIn) : type(typeIn), isExpr(isExprIn), line(lineIn), column(columnIn) {}
			virtual ~Node() {}

			// Every type of node has a print to console functionality
			virtual void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "Node: " << static_cast<int>(type) << '\n';
			}
		};

		// A vector of pointers to nodes
		/*	It's important that this is a vector of POINTERS. The ownership of a node pointer may
			be transferred, for example it may start as part of the original node list but end up
			as the right side of an addition expression. It is easier to transfor a pointer to the
			node than to have to copy the entire node.
		*/
		class NodeList : public std::list<Node*> {
		public:
			void print(std::ostream& stream, int indent) {
				for (auto ptr = begin(); ptr != end(); ptr++) {
					(*ptr)->print(stream, indent);
				}
			}

			// Deletes the nodes that it holds the pointers to (those nodes have destructors that delete their children)
			~NodeList() {
				for (auto ptr = begin(); ptr != end(); ptr++) {
					delete (*ptr);
				}
			}
		};

		//

		// A set of matching parentheses, with a list of all of the nodes contained within
		struct NodeParenGroup : public Node {
			NodeList nodeList;

			NodeParenGroup() : Node(NodeType::PAREN_GROUP, -1, -1) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << '(' << '\n';
				nodeList.print(stream, indent + 1);
				stream << std::string(indent, '\t') << ')' << '\n';
			}
		};

		// A set of matching square brackets, with a list of all of the nodes contained within
		struct NodeSquareGroup : public Node {
			NodeList nodeList;

			NodeSquareGroup() : Node(NodeType::SQUARE_GROUP, -1, -1) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << '[' << '\n';
				nodeList.print(stream, indent + 1);
				stream << std::string(indent, '\t') << ']' << '\n';
			}
		};

		// A set of matching curly brackets, with a list of all of the nodes contained within
		struct NodeCurlyGroup : public Node {
			NodeList nodeList;

			NodeCurlyGroup() : Node(NodeType::CURLY_GROUP, -1, -1) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << '{' << '\n';
				nodeList.print(stream, indent + 1);
				stream << std::string(indent, '\t') << '}' << '\n';
			}
		};

		//

		// A subclass of node for anything that evaluates to an expression
		struct Expr : public Node {
			const EvalType evalType;

			Expr(NodeType typeIn, EvalType evalTypeIn, int lineIn, int columnIn) : Node(typeIn, true, lineIn, columnIn), evalType(evalTypeIn) {}
		};

		// A subclass of node that acts as a wrapper for a token
		struct NodeToken : public Node {
			// Essentially a pointer to a token
			TokenList::iterator token;

			NodeToken(TokenList::iterator tokenIn) : Node(NodeType::TOKEN, tokenIn->line, tokenIn->column), token(tokenIn) {}
			// Note that the Node does not destroy the Token. That's because all of the tokens are
			// still destroyed normally by the TokenList that they are a part of

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "[Token] " << static_cast<int>(token->type) << '\n';
			}
		};

		// A raw integer node
		struct ExprInt : public Expr {
			int_t int_;

			ExprInt(int_t intIn) : Expr(NodeType::INT, EvalType(PrimType::INT), -1, -1), int_(intIn) {}
			ExprInt(NoDestructToken* token) : Expr(NodeType::INT, EvalType(PrimType::INT), token->line, token->column), int_(token->int_) {}
			// TODO : Change this to take a Token as input, and adopt the line/column automatically

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "[Int] " << int_ << '\n';
			}
		};

		// A raw float node
		struct ExprFloat : public Expr {
			float_t float_;

			ExprFloat(float_t floatIn) : Expr(NodeType::FLOAT, EvalType(PrimType::FLOAT), -1, -1), float_(floatIn) {}
			ExprFloat(NoDestructToken* token) : Expr(NodeType::FLOAT, EvalType(PrimType::FLOAT), token->line, token->column), float_(token->float_) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "[Float] " << float_ << '\n';
			}
		};

		// A raw bool node
		struct ExprBool : public Expr {
			bool_t bool_;

			ExprBool(bool_t boolIn) : Expr(NodeType::BOOL, EvalType(PrimType::BOOL), -1, -1), bool_(boolIn) {}
			ExprBool(NoDestructToken* token, bool_t boolIn) : Expr(NodeType::BOOL, EvalType(PrimType::BOOL), token->line, token->column), bool_(boolIn) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "[Bool] " << static_cast<int>(bool_) << '\n';
			}
		};

		// A raw char node
		struct ExprChar : public Expr {
			char_t char_;

			ExprChar(char_t charIn) : Expr(NodeType::CHAR, EvalType(PrimType::CHAR), -1, -1), char_(charIn) {}
			ExprChar(NoDestructToken* token) : Expr(NodeType::CHAR, EvalType(PrimType::CHAR), token->line, token->column), char_(token->char_) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "[Char] " << char_ << '\n';
			}
		};

		//

		// An node representing an identifier
		// TODO: At some point there'll be a dictionary of declared identifiers that this should refer to
		// instead of holding its own data
		struct ExprIdentifier : public Expr {
			int id;

			ExprIdentifier(int idIn) : Expr(NodeType::IDENTIFIER, EvalType(PrimType::UNKNOWN), -1, -1), id(idIn) {}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "[" << evalType.printName() << "] ID: " << id << '\n';
			}
		};

		//

		// A node representing a type cast
		struct ExprCast : public Expr {
			Expr* source;

			ExprCast(Expr* sourceIn, EvalType typeIn) : Expr(NodeType::CAST, typeIn, sourceIn->line, sourceIn->column), source(sourceIn) {}
			~ExprCast() {
				delete source;
			}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "[" << evalType.printName() << "] Cast from\n";
				source->print(stream, indent + 1);
			}
		};

		// A node representing a binary infix operation
		struct ExprBinop : public Expr {
			Expr* left;
			Expr* right;
			OpType opType;

			ExprBinop(Expr* leftIn, Expr* rightIn, OpType opTypeIn, EvalType typeIn, int lineIn, int columnIn) : Expr(NodeType::BINOP, typeIn, lineIn, columnIn), left(leftIn), right(rightIn), opType(opTypeIn) {}
			~ExprBinop() {
				delete left;
				delete right;
			}

			void print(std::ostream& stream, int indent) {
				stream << std::string(indent, '\t') << "[" << evalType.printName() << "] Binop " << opTypeNames[static_cast<int>(opType)] << '\n';
				left->print(stream, indent + 1);
				stream << std::string(indent + 1, '\t') << "------\n";
				right->print(stream, indent + 1);
			}
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
			UNKNOWN,
			STRING_TOO_LONG,
			INVALID_NUMBER,
			INVALID_CLOSING_PAREN,
			INVALID_CLOSING_SQUARE,
			INVALID_CLOSING_CURLY,
			MISSING_CLOSING_PAREN,
			MISSING_CLOSING_SQUARE,
			MISSING_CLOSING_CURLY,
			BINOP_MISSING_EXPRESSION,
			BINOP_ILLEGAL_PATTERN,

			OUT_OF_REGISTERS,
			UNKNOWN_CAST,
			UNKNOWN_BINOP
		};

		static constexpr const char* const errorStrings[] = {
			"I don't really know how this happened but it shouldn't have",
			"String too long",
			"Invalid number",
			"Invalid closing parenthesis",
			"Invalid closing square bracket",
			"Invalid closing curly bracket",
			"Missing a closing parenthesis",
			"Missing a closing square bracket",
			"Missing a closing curly bracket",
			"Binop is missing an expression on one or both sides",
			"No binop pattern exists for the given operand types",

			"It appears that we require more registers than are avaliable... I guess I'll have to fix that eventually",
			"Attempting to cast to or from an unknown type (don't know how this happened)",
			"Attempting a binop with an unknown type, or a boolean (don't know how this happened)"
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
	// Stuff for the bytecode generator

	// Keeps track of which registers are in use or not
	struct RegManager {
		bool wordsActive[register_::NUM_WORD_REGISTERS];
		bool bytesActive[register_::NUM_BYTE_REGISTERS];

		RegManager();

		reg_t getWord();

		void freeWord(reg_t reg);

		reg_t getByte();
		void freeByte(reg_t reg);

		void free(reg_t reg);

		bool isWord(reg_t reg);

		bool isByte(reg_t reg);
	};

	// Gets the opcode for a cast. Used as castOpcodes[EvalType:: *source* ][EvalType:: *target* ]
	// -1 means error (we have an unknown type for some reason)
	// -2 means do nothing (no cast necessary)
	// -3, -4, -5 are special-case conversions to bool 
	constexpr int castOpcodes[5][5] = {
		{-1,	-1,				-1,				-1,				-1},
		{-1,	-2,				opcode::I_TO_F,	opcode::I_TO_C,	-3},
		{-1,	opcode::F_TO_I,	-2,				opcode::F_TO_C,	-4},
		{-1,	opcode::C_TO_I,	opcode::C_TO_F,	-2,				-5},
		{-1,	opcode::C_TO_I,	opcode::C_TO_F,	-2,				-2}
	};

	// Gets the opcode for a binop. Used as binopOpcodes[OpType:: *type* ][EvalType:: *operand type*]
	constexpr int binopOpcodes[4][5] = {
		{-1, opcode::I_ADD, opcode::F_ADD, -1, opcode::C_ADD },
		{-1, opcode::I_SUB, opcode::F_SUB, -1, opcode::C_SUB },
		{-1, opcode::I_MUL, opcode::F_MUL, -1, opcode::C_MUL },
		{-1, opcode::I_DIV, opcode::F_DIV, -1, opcode::C_DIV },
	};

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Functions

	int compile(const char* const& inputPath, const char* const& outputPath, CompilerSettings& settings);
	int compile_(std::iostream& inputFile, std::iostream& outputFile, CompilerSettings& settings, std::ostream& stream);

	int tokenize(TokenList& tokenList, std::iostream& file, CompilerSettings& compileSettings, std::ostream& stream);
	int parseNumber(NoDestructToken& token);

	int constructAST(AST::NodeList& outputList, TokenList& tokenList, CompilerSettings& compileSettings, std::ostream& stream);
	int condenseAST(AST::NodeList& list, AST::NodeList& subList, AST::NodeList::iterator start, AST::NodeType type, CompilerSettings& compileSettings, std::ostream& stream);

	int makeBytecode(AST::NodeList& list, std::iostream& outputFile, CompilerSettings& compileSettings, std::ostream& stream);
	reg_t makeExprBytecode(AST::Expr* expr, RegManager& reg, std::iostream& outputFile, int& byteCounter, CompilerSettings& compileSettings, std::ostream& stream);
	reg_t makeCastBytecode(AST::ExprCast* expr, RegManager& reg, std::iostream& outputFile, int& byteCounter, CompilerSettings& compileSettings, std::ostream& stream);
	reg_t makeBinopBytecode(AST::ExprBinop* expr, RegManager& reg, std::iostream& outputFile, int& byteCounter, CompilerSettings& compileSettings, std::ostream& stream);
}