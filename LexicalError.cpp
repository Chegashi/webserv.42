#include "LexicalError.hpp"
#include "utils.hpp"
#include "colors.hpp"
#include "Component.hpp"

LexicalError::LexicalError(std::string _error, std::string programName, std::string file, int line, int col, bool higher_verbose, const Component& where, int argError) {
	(void)higher_verbose;
	error = BOLD + programName + ": " + BOLD_RED + "lexical_error" + RESET + BOLD + ": " + file + ":" + to_string(line) + ":" + to_string(col) + " " + _error + RESET;
	if (higher_verbose) {
		error += '\n';
		int ccol = 1;
		error += FAINT_GRAY;
		for (int i = 1; i < where.depth(); i++) {
			error += "→   ";
			ccol += 4;
		}
		error += RESET;
		error += (where.isContext() ? BABY_PINK : BABY_BLUE) + where.name() + RESET;
		if (argError > 0) {
			ccol += where.name().length() + 1;
		}
		error += (where.isContext() ? CLASS_GREEN : CONST_BLUE);
		for (std::vector<std::string>::const_iterator it = where.attr().begin(); it != where.attr().end(); it++) {
			if (argError > 1 + it - where.attr().begin()) {
				ccol += it->length() + 1;
			}
			error += ' ' + *it;
		}
		error += RESET;
		error += (where.isContext() ? " {" : ";");
		error += '\n';
		for (int i = 1; i < ccol; i++) {
			error += ' ';
		}
		error += "^";
		int argErrorLength = argError ? where.attr(argError - 1).length() : where.name().length();
		for (int i = 1; i < argErrorLength; i++) {
			error += "~";
		}
	}
}

LexicalError::~LexicalError() throw() {}

LexicalError::LexicalError(const LexicalError &ref): error(ref.error) {}

LexicalError &LexicalError::operator=(const LexicalError &ref) {
	error = ref.error;
	return *this;
}

const char *LexicalError::what() const throw() {
	return error.c_str();
}