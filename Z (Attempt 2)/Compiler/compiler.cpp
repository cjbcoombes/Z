#include "compiler.h"

int compiler::compile(const char* const& inputPath, const char* const& outputPath, CompilerSettings& compileSettings) {
	using std::cout;
	cout << "Attempting to compile file \"" << inputPath << "\" into output file \"" << outputPath << "\"\n";

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

	TokenList tokenList;

	tokenize(tokenList, inputFile, stream);

	return 0;
}

int compiler::tokenize(TokenList& tokenList, std::iostream& file, std::ostream& stream) {
	file.clear();
	file.seekg(0, std::ios::beg);

	tokenList.empty();
	tokenList.clear();

	// String
	char str[MAX_STR_SIZE + 1];
	int strlen = 0;
	char c = -1;
	int line = 0;
	int column = -1;

	// Booleans
	bool end = false;
	bool isComment = false;
	bool isBlockComment = false;
	bool isStr = false;
	bool isChar = false; // TODO!
	bool isEscaped = false;
	bool isNumber = false;
	bool hasDecimal = false;

	int i;
	int token1Index = -1;
	int token2Index = -1;
	int primType = -1;
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

		// Dealing with strings and comments
		if (isStr) {
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
				// nothing on c == '"'

				isEscaped = false;
			} else {
				if (c == '"') {
					isStr = false;
					str[strlen] = '\0';
					tokenList.emplace_back(TokenType::STRING, line, column, new std::string(str));
					token1Index = -1;
					strlen = 0;
					continue;
				}

				if (c == '\\') {
					isEscaped = true;
					continue;
				}
			}

			if (strlen >= MAX_STR_SIZE) {
				throw CompilerException(CompilerException::STRING_TOO_LONG, line, column);
			}
			str[strlen++] = c;
			continue;
		} else {
			if (isBlockComment) {
				if (strlen == 1 && c == '/') isBlockComment = false;
				else if (c == '*') strlen = 1;
				else strlen = 0;
				continue;
			}

			if (isComment) {
				if (c == '\n') isComment = false;
				continue;
			}

			if (isNumber) {
				// TODO : else (quit number) and determining the base
				if (strlen == 1 && str[0] == '0' && (c == 'x' || c == 'b')) {
					str[strlen++] = c;
					continue;
				} if (('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || (c == '.' && !hasDecimal)) {
					if (c == '.') hasDecimal = true;
					str[strlen++] = c;
					continue;
				} else {

					// Intentionally continue
				}
			}

			// Redundant?
			/*if (strlen == 1 && str[0] == '/' && c == '/') {
				isComment = true;
				continue;
			}

			// Redundant?
			if (strlen == 1 && str[0] == '/' && c == '*') {
				isBlockComment = true;
				strlen = 0;
				continue;
			}*/
		}

		if (token1Index >= 0) {
			// Check for token2s
			token2Index = -1;
			for (i = 0; i < numToken2s; i++) {
				if (c == token2s[i].second && token1Index == static_cast<int>(token2s[i].first)) {
					token2Index = i + firstToken2;
					break;
				}
			}

			if (token2Index >= 0) {
				if (token2Index == static_cast<int>(TokenType::SLASH_SLASH)) isComment = true;
				if (token2Index == static_cast<int>(TokenType::SLASH_STAR)) isBlockComment = true;
				else tokenList.emplace_back(static_cast<TokenType>(token2Index), line, column);
				token1Index = -1;
				strlen = 0;
				continue;
			} else {
				tokenList.emplace_back(static_cast<TokenType>(token1Index), line, column);
			}
		}

		token1Index = -1;
		for (i = 0; i < numToken1s; i++) {
			if (c == token1s[i]) {
				token1Index = i + firstToken1;
				break;
			}
		}


		// At the end of a token:
		if (token1Index >= 0 || c == ' ' || c == '\n' || c == '\t' || c == '"' || ('0' <= c && c <= '9') || isNumber || end) {
			if (strlen != 0) {
				str[strlen] = '\0';

				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

				if (isNumber) {
					tokenList.emplace_back(TokenType::NUM_UNIDENTIFIED, line, column, new std::string(str));
				} else {
					primType = stringMatchAt(str, primTypes, numPrimTypes);
					if (primType >= 0) {
						tokenList.emplace_back(TokenType::PRIMITIVE_TYPE, line, column, static_cast<PrimitiveType>(primType));
					} else {
						keyword = stringMatchAt(str, keywords, numKeywords);
						if (keyword >= 0) {
							tokenList.emplace_back(static_cast<TokenType>(keyword + firstKeyword), line, column);
						} else {
							tokenList.emplace_back(TokenType::IDENTIFIER, line, column, new std::string(str));
						}
					}
				}
			}

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

	for (auto ptr = tokenList.begin(); ptr < tokenList.end(); ptr++) {
		stream << ptr->line << "  " << ptr->column;
		if (ptr->type == TokenType::NUM_UNIDENTIFIED) stream << "  #: " << *(ptr->str);
		else if (ptr->hasStr) stream << "  ID: " << *(ptr->str);
		else stream << "  Type: " << static_cast<int>(ptr->type);
		stream << '\n';
	}

	return 0;
}