#include "compiler.h"

int compiler::compile(const char* const& inputPath, const char* const& outputPath, CompilerSettings& compileSettings) {
	using std::cout;
	cout << "Attempting to compile file \"" << inputPath << "\" into output file \"" << outputPath << "\"\n";

	// Open streams for the input and output files
	std::fstream inputFile, outputFile;

	inputFile.open(inputPath, std::ios::in);
	if (!inputFile.is_open()) {
		cout << IO_ERR "Could not open file \"" << inputPath << "\"" IO_NORM IO_END;
		return 1;
	}

	outputFile.open(outputPath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!outputFile.is_open()) {
		cout << IO_ERR "Could not open file \"" << outputPath << "\"" IO_NORM IO_END;
		return 1;
	}

	try {
		return compile_(inputFile, outputFile, compileSettings, std::cout);
	} catch (CompilerException& e) {
		cout << IO_ERR "Error during compilation at LINE " << e.line << ", COLUMN " << e.column << " : " << e.what() << IO_NORM IO_END;
	} catch (std::exception& e) {
		cout << IO_ERR "An unknown error ocurred during compilation. This error is most likely an issue with the c++ compiler code, not your code. Sorry. The provided error message is as follows:\n" << e.what() << IO_NORM IO_END;
	}

	return 1;
}

int compiler::compile_(std::iostream& inputFile, std::iostream& outputFile, CompilerSettings& compileSettings, std::ostream& stream) {
	// Flags/settings
	const bool isDebug = compileSettings.flags.hasFlags(FLAG_DEBUG);

	// Tokenize
	TokenList tokenList;
	tokenize(tokenList, inputFile, stream);

	// Construct AST
	AST::Tree tree;
	constructAST(tree, tokenList, stream);

	return 0;
}

int compiler::tokenize(TokenList& tokenList, std::iostream& file, std::ostream& stream) {
	// Go to the start of the file
	file.clear();
	file.seekg(0, std::ios::beg);

	tokenList.clear();

	// String variables
	// Stores the current working string
	char str[MAX_STR_SIZE + 1];
	// Stores the length of the working string
	int strlen = 0;
	// Stores the current char being read from the stream
	char c = -1;

	// Keeps track of lines and columns for error tracking
	int line = 0;
	int column = -1;

	// Various flags that are used in the loop
	// end is for the end of the string
	bool end = false;
	// The rest are mostly self-explanatory
	bool isComment = false;
	bool isBlockComment = false;
	bool isStr = false;
	bool isChar = false; // TODO!
	bool isEscaped = false;
	bool isNumber = false;
	bool hasDecimal = false;

	// An iterator for loops
	int i;
	// Keeping track of whether or not certain tokens are matching
	int token1Index = -1;
	int token2Index = -1;
	// Checking if a keyword matches
	int keyword = -1;

	// Da big loop
	while (!end) {
		// Update char and file pos
		if (c == '\n') {
			line++;
			column = 0;
		} else {
			column++;
		}
		file.get(c);
		if (file.eof()) end = true;

		// Dealing with strings
		if (isStr) {
			// Match certain escape sequences
			if (isEscaped) {
				if (c == 'n') {
					c = '\n';
					line--;
				} else if (c == 'c') {
					c = '\033';
				} else if (c == '0') {
					c = '\0';
				} else if (c == 't') {
					c = '\t';
				}
				// For anything else the escape doesn't change anything
				// That means for example \" will become " but it won't terminate the string

				isEscaped = false;
			} else {
				// End of string
				if (c == '"') {
					isStr = false;
					// Null-terminate string
					str[strlen] = '\0';
					// Add to the token list
					tokenList.emplace_back(TokenType::STRING, line, column, new std::string(str));
					token1Index = -1;
					strlen = 0;
					continue;
				}

				// Start an escape sequence
				if (c == '\\') {
					isEscaped = true;
					continue;
				}
			}

			// Check if string is too long
			if (strlen >= MAX_STR_SIZE) {
				throw CompilerException(CompilerException::STRING_TOO_LONG, line, column);
			}
			// If nothing special happened, just add the char to the current string
			str[strlen++] = c;
			// Skip any processing since we aren't at the end of a token
			continue;
		} else { // Not a string
			// If it's a block comment, check to see if it's ended
			if (isBlockComment) {
				if (strlen == 1 && c == '/') isBlockComment = false;
				else if (c == '*') strlen = 1;
				else strlen = 0;
				continue;
			}

			// If it's a normal comment, check to see if it's ended
			if (isComment) {
				if (c == '\n') isComment = false;
				continue;
			}

			// If it's a number
			if (isNumber) {
				// Check for special base-specifying prefixes like 0x and 0b
				if (strlen == 1 && str[0] == '0' && (c == 'x' || c == 'b')) {
					str[strlen++] = c;
					continue;
				} else if (!end && ('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || (c == '.' && !hasDecimal)) {
					// Add characters to the number 
					if (c == '.') hasDecimal = true;
					str[strlen++] = c;
					continue;
				} else {
					// Otherwise the number is over
					// Intentionally continue to token processing
				}
			}
		}

		// If the PREVIOUS token was matched a token1
		if (token1Index >= 0) {
			// Check to see if that pairs with a current char to make a token2
			token2Index = -1;
			for (i = 0; i < numToken2s; i++) {
				if (c == token2s[i].second && token1Index == static_cast<int>(token2s[i].first)) {
					token2Index = i + firstToken2;
					break;
				}
			}

			// If a token2 matched
			if (token2Index >= 0) {
				// Start a comment if it's the start of a comment
				if (token2Index == static_cast<int>(TokenType::SLASH_SLASH)) isComment = true;
				if (token2Index == static_cast<int>(TokenType::SLASH_STAR)) isBlockComment = true;
				// Otherwise add the token to the list
				else tokenList.emplace_back(static_cast<TokenType>(token2Index), line, column);
				token1Index = -1;
				strlen = 0;
				continue;
			} else {
				// If no token2 was matched make sure to add the token1 from the previous loop
				tokenList.emplace_back(static_cast<TokenType>(token1Index), line, column);
			}
		}

		// Check to see if the current char is a token1
		token1Index = -1;
		for (i = 0; i < numToken1s; i++) {
			if (c == token1s[i]) {
				token1Index = i + firstToken1;
				break;
			}
		}


		// At the end of a token...
		// Note that this essentially parses the token that ended with the PREVIOUS char,
		// while the current char will be part of the next token
		if (token1Index >= 0 || c == ' ' || c == '\n' || c == '\t' || c == '"' || ('0' <= c && c <= '9') || isNumber || end) {
			if (strlen != 0) {
				str[strlen] = '\0';

				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

				// Parse it as a number if it's a number
				if (isNumber) {
					tokenList.emplace_back(TokenType::NUM_UNIDENTIFIED, line, column, new std::string(str));
					// parseNumber modifies its input, so in this case it will change the internal state of the
					// token from a string to an actual number, or throw an error
					if (parseNumber(tokenList.back())) {
						throw CompilerException(CompilerException::INVALID_NUMBER, line, column);
					}
				} else {
					// If it's not a number, check for keyword matches
					keyword = stringMatchAt(str, keywords, numKeywords);
					if (keyword >= 0) {
						tokenList.emplace_back(static_cast<TokenType>(keyword + firstKeyword), line, column);
					} else {
						// Otherwise it must be an identifier
						tokenList.emplace_back(TokenType::IDENTIFIER, line, column, new std::string(str));
					}
				}
			}

			// Reset trackers and check for flags that need to be set for the next token
			isNumber = false;
			strlen = 0;
			if (c == '"') {
				isStr = true;
				isEscaped = false;
			} else if ('0' <= c && c <= '9') {
				isNumber = true;
				hasDecimal = false;
				str[strlen++] = c;
			}
			continue;
		}

		// Add char to string
		if (strlen >= MAX_STR_SIZE) {
			throw CompilerException(CompilerException::STRING_TOO_LONG, line, column);
		}
		str[strlen++] = c;
	}

	///* Print Tokens
	for (auto ptr = tokenList.begin(); ptr < tokenList.end(); ptr++) {
		stream << std::setw(3) << ptr->line << "  " << std::setw(3) << ptr->column;
		if (ptr->type == TokenType::NUM_UNIDENTIFIED) stream << "         #: " << *(ptr->str);
		else if (ptr->type == TokenType::NUM_INT) stream << "       Int: " << (ptr->int_);
		else if (ptr->type == TokenType::NUM_FLOAT) stream << "     Float: " << (ptr->float_);
		else if (ptr->type == TokenType::STRING) stream << "    String: " << *(ptr->str);
		else if (ptr->hasStr) stream << "        ID: " << *(ptr->str);
		else stream << "            " << static_cast<int>(ptr->type);
		stream << '\n';
	}
	/**/

	return 0;
}

int compiler::parseNumber(NoDestructToken& token) {
	if (token.type != TokenType::NUM_UNIDENTIFIED) return 1;
	// String to be turned into a number
	const char* str = (*(token.str)).c_str();

	// Base 10 by default
	int_t base = 10;

	// Return error if the string is empty
	if (str[0] == '\0') {
		return 1;
	} else if (str[0] == '0') { // Check for base-setting pattern at the start like 0x or 0b
		char t = str[1];
		if (t < '0' || t > '9') {
			str += 2;
			switch (t) {
				case 'x':
					base = 16;
					break;

				case 'b':
					base = 2;
					break;

				case '.':
				case '\0':
					str--;
					break;

				case 'd':break;

				default:
					return 1;
			}
		}
	}

	// Could be an int or a float, we don't know yet
	int_t int_ = 0;
	float_t float_ = 0;
	// A multiplying factor to help with float parsing
	float_t factor = 1;
	char c;
	// Loops through the string
	while ((c = *str) != '\0') {
		// A decimal point indicates a float
		if (c == '.') {
			float_ = int_;
			str++;
			// Loops through the rest of the string (after the decimal)
			while ((c = *str) != '\0') {
				factor *= base;

				// Applies digit with appropriate scaling
				if ('0' <= c && c <= '9') {
					if (c - '0' >= base) return 1;
					float_ += (c - '0') / factor;
				} else if ('A' <= c && c <= 'Z') {
					if (c - 'A' + 10 >= base) return 1;
					float_ += (c - 'A' + 10) / factor;
				} else if ('a' <= c && c <= 'z') {
					if (c - 'a' + 10 >= base) return 1;
					float_ += (c - 'a' + 10) / factor;
				} else {
					return 1;
				}

				str++;
			}

			// Replaces the token's string with the number
			delete token.str;
			token.hasStr = false;
			token.float_ = float_;
			token.type = TokenType::NUM_FLOAT;

			return 0;
		}


		// Applies digit with appropriate scaling
		int_ *= base;

		if ('0' <= c && c <= '9') {
			if (c - '0' >= base) return 1;
			int_ += c - '0';
		} else if ('A' <= c && c <= 'Z') {
			if (c - 'A' + 10 >= base) return 1;
			int_ += c - 'A' + 10;
		} else if ('a' <= c && c <= 'z') {
			if (c - 'a' + 10 >= base) return 1;
			int_ += c - 'a' + 10;
		} else {
			return 1;
		}

		str++;
	}

	// Replaces the token's string with the number
	delete token.str;
	token.hasStr = false;
	token.int_ = int_;
	token.type = TokenType::NUM_INT;

	return 0;
}

int compiler::constructAST(AST::Tree& tree, TokenList& tokenList, std::ostream& stream) {
	using namespace AST;
	NodeList nodeList;
	// The nodelist starts out filled with wrappers of all of the tokens
	for (auto ptr = tokenList.begin(); ptr < tokenList.end(); ptr++) {
		nodeList.emplace_back(new NodeToken(ptr));
	}

	// First Pass: create nodes for tokens that should have their own node
	// This includes NUMBERS, IDENTIFIERS, ... more (TODO)
	for (auto ptr = nodeList.begin(); ptr != nodeList.end(); ptr++) {
		// OK to cast since we know they're all NodeToken right now
		NodeToken* origNode = static_cast<NodeToken*>(*ptr);
		Node* newNode = nullptr;
		TokenType type = origNode->token->type;

		// Create appropriate nodes depending on the type (pretty self-explanatory)
		switch (type) {
			case TokenType::NUM_INT:
				newNode = new ExprInt(origNode->token->int_);
				break;

			case TokenType::NUM_FLOAT:
				newNode = new ExprFloat(origNode->token->float_);
				break;

			case TokenType::IDENTIFIER:
				// Ownership of string pointer is effectively transferred
				newNode = new ExprIdentifier(origNode->token->str);
				origNode->token->hasStr = false;
				origNode->token->str = nullptr;
				break;

			case TokenType::TRUE:
				newNode = new ExprBool(1);
				break;

			case TokenType::FALSE:
				newNode = new ExprBool(0);
				break;
		}

		// If there's a new node, delete the old one and replace it in the list
		if (newNode) {
			delete origNode;
			// Note that there is no construction/destruction here, we are only moving a pointer around
			(*ptr) = newNode;
		}
	}

	// The node list that the AST output will be constructed into
	NodeList outputList;

	// Call the AST creator (this is a recursive function)
	condenseAST(nodeList, outputList, nodeList.begin(), NodeType::NONE);

	// Print the output list (more like a tree at this point)
	outputList.print(stream, 0);

	return 0;
}

int compiler::condenseAST(AST::NodeList& inputList, AST::NodeList& outputList, AST::NodeList::iterator start, AST::NodeType type) {
	using namespace AST;
	// parameter type will be NONE, PAREN_GROUP, SQUARE_GROUP, CURLY_GROUP

	// The main pointer that will iterate through the list
	NodeList::iterator ptr = start;
	// A helper pointer (honestly I couldn't think of a better name for this variable thatn pptr)
	NodeList::iterator pptr;
	// These may not all be needed but I can't declare them inside the loop/switch
	NodeParenGroup* parenGroup;
	NodeSquareGroup* squareGroup;
	NodeCurlyGroup* curlyGroup;

	// Loop through the input list, starting at the start parameter
	// This loop is just to find and separate closed parentheses/bracket groups
	while (ptr != inputList.end()) {
		// If it's a token...
		if ((*ptr)->type == NodeType::TOKEN) {
			// Switch through some possible tokens
			switch (static_cast<NodeToken*>(*ptr)->token->type) {
				// Opening parenthesis
				case TokenType::LEFT_PAREN:
					// Create a new paren group
					parenGroup = new NodeParenGroup();
					// Call the AST creator, recursively, at the new start location
					// It will output to the parengroup
					condenseAST(inputList, parenGroup->nodeList, std::next(ptr), NodeType::PAREN_GROUP);

					// If the parens contain only one item, and that evaluates as an expression,
					// then insert the expression and delete the group
					if (parenGroup->nodeList.size() == 1 && (*(parenGroup->nodeList.begin()))->isExpr) {
						inputList.insert(ptr, *(parenGroup->nodeList.begin()));
						(*(parenGroup->nodeList.begin())) = new Node();
						delete parenGroup;
					} else { // Otherwise just insert the group
						inputList.insert(ptr, parenGroup);
					}

					// Currently: pointer still points to the opening paren,
					// and the new stuff has been inserted before that point

					// Delete the opening paren node
					delete (*ptr);
					// Get and delete the next node, which is the closing paren
					pptr = std::next(ptr);
					delete (*pptr);
					// Remove the now-dead pointers from the list
					pptr++;
					ptr = inputList.erase(ptr, pptr);
					// ptr now points to the next element. This moves it back to point to the
					// newly inserted stuff
					ptr--;
					break;

					// Opening square bracket, same logic as the parentheses
				case TokenType::LEFT_SQUARE:
					squareGroup = new NodeSquareGroup();
					condenseAST(inputList, squareGroup->nodeList, std::next(ptr), NodeType::SQUARE_GROUP);

					inputList.insert(ptr, squareGroup);
					delete (*ptr);
					pptr = std::next(ptr);
					delete (*pptr);
					pptr++;
					ptr = inputList.erase(ptr, pptr);
					ptr--;
					break;

					// Opening curly bracket, same logic as the parentheses
				case TokenType::LEFT_CURLY:
					curlyGroup = new NodeCurlyGroup();
					condenseAST(inputList, curlyGroup->nodeList, std::next(ptr), NodeType::CURLY_GROUP);

					inputList.insert(ptr, curlyGroup);
					delete (*ptr);
					pptr = std::next(ptr);
					delete (*pptr);
					pptr++;
					ptr = inputList.erase(ptr, pptr);
					ptr--;
					break;

					// Closing parenthesis
				case TokenType::RIGHT_PAREN:
					// If we are looking for a closing parenthesis...
					if (type == NodeType::PAREN_GROUP) {
						// We now have a list with no parentheses/brackets, so it can be processed
						outputList.splice(outputList.begin(), inputList, start, ptr);
						goto process;
					} else { // Otherwise there is an extra closing parenthesis and that's an error
						throw CompilerException(CompilerException::INVALID_CLOSING_PAREN, static_cast<NodeToken*>(*ptr)->token->line, static_cast<NodeToken*>(*ptr)->token->column);
					}
					break;

					// Closing square bracket
				case TokenType::RIGHT_SQUARE:
					// If we are looking for a closing bracket...
					if (type == NodeType::SQUARE_GROUP) {
						// We now have a list with no parentheses/brackets, so it can be processed
						outputList.splice(outputList.begin(), inputList, start, ptr);
						goto process;
					} else { // Otherwise there is an extra closing bracket and that's an error
						throw CompilerException(CompilerException::INVALID_CLOSING_SQUARE, static_cast<NodeToken*>(*ptr)->token->line, static_cast<NodeToken*>(*ptr)->token->column);
					}
					break;

					// Closing curly bracket
				case TokenType::RIGHT_CURLY:
					// If we are looking for a closing bracket...
					if (type == NodeType::CURLY_GROUP) {
						// We now have a list with no parentheses/brackets, so it can be processed
						outputList.splice(outputList.begin(), inputList, start, ptr);
						goto process;
					} else { // Otherwise there is an extra closing bracket and that's an error
						throw CompilerException(CompilerException::INVALID_CLOSING_CURLY, static_cast<NodeToken*>(*ptr)->token->line, static_cast<NodeToken*>(*ptr)->token->column);
					}
					break;
			}
		}
		ptr++;
	}

	// This means that we either have a missing parenthesis/bracket (throws an error) or we've reached the end of the list
	switch (type) {
		case NodeType::NONE:
			outputList.splice(outputList.begin(), inputList, start, ptr);
			break;
		case NodeType::PAREN_GROUP:
			// TODO: Make all nodes store line and column info
			// throw CompilerException(CompilerException::INVALID_CLOSING_PAREN, static_cast<NodeToken*>(*(--ptr))->token->line, static_cast<NodeToken*>(*(--ptr))->token->column);
			throw CompilerException(CompilerException::MISSING_CLOSING_PAREN, -1, -1);
			break;
		case NodeType::SQUARE_GROUP:
			throw CompilerException(CompilerException::MISSING_CLOSING_SQUARE, -1, -1);
			break;
		case NodeType::CURLY_GROUP:
			throw CompilerException(CompilerException::MISSING_CLOSING_CURLY, -1, -1);
			break;
	}
	

process:
	bool flag = false;
	ExprBinop* binop;

	// Pass 1: Multiplication and Division
	for (ptr = outputList.begin(); ptr != outputList.end(); ptr++) {
		switch ((*ptr)->type) {
			case NodeType::TOKEN:
				flag = false;
				switch (static_cast<NodeToken*>(*ptr)->token->type) {
					case TokenType::STAR:
						flag = true;
					case TokenType::SLASH:
						pptr = std::next(ptr);
						if (ptr == outputList.begin() || pptr == outputList.end() || !((*pptr)->isExpr) || !((*std::prev(ptr))->isExpr)) {
							throw CompilerException(CompilerException::BINOP_MISSING_EXPRESSION, static_cast<NodeToken*>(*ptr)->token->line, static_cast<NodeToken*>(*ptr)->token->column);
						}
						//std::cout << "[" << (flag ? "*" : "/") << "]";
						// TODO : Determine expression type and apply appropriate casting
						binop = new ExprBinop(static_cast<Expr*>(*std::prev(ptr)), static_cast<Expr*>(*pptr), flag ? OpType::MULT : OpType::DIV, ExprType::UNKNOWN);
						delete (*ptr);
						ptr = outputList.erase(std::prev(ptr), std::next(pptr));
						ptr = outputList.insert(ptr, binop);
						break;
				}
				break;
		}
	}

	// Pass 2: Addition and Subtraction
	for (ptr = outputList.begin(); ptr != outputList.end(); ptr++) {
		switch ((*ptr)->type) {
			case NodeType::TOKEN:
				flag = false;
				switch (static_cast<NodeToken*>(*ptr)->token->type) {
					case TokenType::PLUS:
						flag = true;
					case TokenType::DASH:
						pptr = std::next(ptr);
						if (ptr == outputList.begin() || pptr == outputList.end() || !((*pptr)->isExpr) || !((*std::prev(ptr))->isExpr)) {
							throw CompilerException(CompilerException::BINOP_MISSING_EXPRESSION, static_cast<NodeToken*>(*ptr)->token->line, static_cast<NodeToken*>(*ptr)->token->column);
						}
						//std::cout << "[" << (flag ? "+" : "-") << "]";
						// TODO : Determine expression type and apply appropriate casting
						binop = new ExprBinop(static_cast<Expr*>(*std::prev(ptr)), static_cast<Expr*>(*pptr), flag ? OpType::ADD : OpType::SUB, ExprType::UNKNOWN);
						delete (*ptr);
						ptr = outputList.erase(std::prev(ptr), std::next(pptr));
						ptr = outputList.insert(ptr, binop);
						break;
				}
				break;
		}
	}

	return 0;
}