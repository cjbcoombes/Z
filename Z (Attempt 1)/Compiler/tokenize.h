#include "tokenizedefs.h"

void compile::Tokenize(std::iostream& file, 
			  compile::TokenList& list
		  #ifdef COMP_DEBUG
			  , std::ostream& debug
		  #endif
			  ) {
	char c;
	bool end = false;

	list.clear();
	file.seekg(0);

#ifdef COMP_DEBUG
	debug << STRM_DEFAULT << "[DEBUG] Tokenization debug";
#endif

	while (!end) {
		file.get(c);
		end = file.eof();
	}
}