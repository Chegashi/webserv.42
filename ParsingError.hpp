#ifndef __PARSINGERROR_HPP__
# define __PARSINGERROR_HPP__

# include <string>

class ParsingError : public std::exception {
	private:
		std::string error;
	public:
		ParsingError(std::string _error = "Unknown Error", std::string progName = "webserv", std::string file = "File", int line = 0, int col = 0, bool higher_verbose = true, std::string fileContents = "");
		~ParsingError() throw();
		ParsingError(const ParsingError &ref);
		ParsingError &operator=(const ParsingError &ref);
		const char *what() const throw();
};

#endif
