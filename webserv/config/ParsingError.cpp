#include "ParsingError.hpp"
#include "utils.hpp"
#include "colors.hpp"

ParsingError::ParsingError(std::string _error, std::string programName, std::string file, int line, int col, bool higher_verbose, std::string fContent) {
	(void)higher_verbose;
	error = BOLD + programName + ": " + BOLD_RED + "parsing_error" + RESET + BOLD + ": " + file + ":" + to_string(line) + ":" + to_string(col) + " " + _error + RESET;
	if (higher_verbose) {
		error += '\n';
		int cline = 1;
		std::string::iterator it;
		for (it = fContent.begin(); it != fContent.end(); it++) {
			if (cline == line) {
				break ;
			}
			if (*it == '\n') {
				cline++;
			}
		}
		std::string::iterator tmp = it;
		while (it != fContent.end() && *it != '\n') {
			if (*it == '\t')
				error += "    ";
			else
				error += *it;
			// col += *it == '\t' ? 3 : 0;
			it++;
		}
		error += '\n';
		it = tmp;
		int i = 1;
		while (it != fContent.end() && *it != '\n' && i < col) {
		// for (int i = 1; i < col; i++) {
			error += *it == '\t' ? "    " : " ";
			it++;
			i++;
		}
		error += "^~~~~~";
	}
}

ParsingError::~ParsingError() throw() {}

ParsingError::ParsingError(const ParsingError &ref): error(ref.error) {}

ParsingError &ParsingError::operator=(const ParsingError &ref) {
	error = ref.error;
	return *this;
}

const char *ParsingError::what() const throw() {
	return error.c_str();
}