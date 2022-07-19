#ifndef __PARSINGERROR_HPP__
# define __PARSINGERROR_HPP__

class ParsingError {
	public:
		ParsingError();
		~ParsingError();
		ParsingError(const ParsingError &ref);
		ParsingError &operator=(const ParsingError &ref);
};

#endif
