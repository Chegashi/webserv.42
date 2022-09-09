#ifndef __LOGICALERROR_HPP__
# define __LOGICALERROR_HPP__

# include "Component.hpp"
# include <string>

class LogicalError : public std::exception {
	private:
		std::string error;
	public:
		LogicalError(std::string _error = "Unknown Error", std::string progName = "webserv", std::string file = "File", int line = 0, int col = 0, bool higher_verbose = true, const Component& where = Component(), int argError = 0);
		~LogicalError() throw();
		LogicalError(const LogicalError &ref);
		LogicalError &operator=(const LogicalError &ref);
		const char *what() const throw();
};

#endif
