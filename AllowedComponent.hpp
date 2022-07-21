#ifndef __ALLOWEDCOMPONENT_HPP__
# define __ALLOWEDCOMPONENT_HPP__

# include <string>
# include <vector>

class AllowedComponent {
	private:
		std::string _name;
		bool _isContext;
		std::vector<std::string> _allowedParents;
		int _minAttr;
		int _maxAttr;
	public:
		bool (*attrIsCorrect)(std::string, int);
		AllowedComponent(	std::string __name = "",
							bool __isContext = false,
							std::vector<std::string> __allowedParents = std::vector<std::string>(),
							int __minAttr = 0,
							int __maxAttr = 0,
							bool (*_attrIsCorrect)(std::string, int) = NULL);
		~AllowedComponent();
		AllowedComponent(const AllowedComponent &ref);
		AllowedComponent &operator=(const AllowedComponent &ref);
};

#endif
