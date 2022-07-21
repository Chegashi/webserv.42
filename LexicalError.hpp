#ifndef __LEXICALERROR_HPP__
# define __LEXICALERROR_HPP__

# include "Component.hpp"
# include <string>

class LexicalError : public std::exception {
	private:
		std::string error;
	public:
		LexicalError(std::string _error = "Unknown Error", std::string progName = "webserv", std::string file = "File", int line = 0, int col = 0, bool higher_verbose = true, const Component& where = Component(), int argError = 0);
		~LexicalError() throw();
		LexicalError(const LexicalError &ref);
		LexicalError &operator=(const LexicalError &ref);
		const char *what() const throw();
};

#endif
