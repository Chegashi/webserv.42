#include "ParsingError.hpp"

ParsingError::ParsingError(std::string _error, std::string file, int line, int col) {
	error = file + ":" + std::to_string(line) + ":" + std::to_string(col) + " " + _error;
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