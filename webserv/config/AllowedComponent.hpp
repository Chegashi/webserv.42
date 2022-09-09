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
		void (*attrIsCorrect)(std::string, int);
		AllowedComponent(	std::string __name = "",
							bool __isContext = false,
							std::vector<std::string> __allowedParents = std::vector<std::string>(),
							int __minAttr = 0,
							int __maxAttr = 0,
							void (*_attrIsCorrect)(std::string, int) = NULL);
		~AllowedComponent();
		AllowedComponent(const AllowedComponent &ref);
		AllowedComponent &operator=(const AllowedComponent &ref);
		const std::string &name() const;
		bool isContext() const;
		const std::vector<std::string> &allowedParents() const;
		int minAttr() const;
		int maxAttr() const;
};

#endif
