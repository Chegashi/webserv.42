#ifndef __COMPONENT_HPP__
# define __COMPONENT_HPP__

# include <string>
# include <vector>

# define CONTEXT true
# define DIRECTIVE false

class Component {
	private:
		std::string _name;
		std::string _parentName;
		std::vector<std::string> _attributes;
		bool _isContext;
		std::vector<Component> _children;
		int _line;
		int _col;
		int _depth;
	public:
		Component(std::string __name = "", std::string __parentName = "",
				  std::vector<std::string> __attr = std::vector<std::string>(),
				  bool __isContext = false,
				  std::vector<Component> __children = std::vector<Component>(),
				  int __line = 0, int __col = 0, int __depth = 0);
		Component(const Component &ref);
		~Component();
		Component &operator=(const Component &ref);
		int line() const;
		int depth() const;
		int col() const;
		void setLine(int __line);
		void setCol(int __col);
		void setDepth(int __depth);
		const std::string& name() const;
		const std::string& parentName() const;
		const std::vector<std::string>& attr() const;
		const std::string& attr(int index) const;
		std::vector<Component>& children();
		Component& children(int index);
		void appendAttr(const std::string &str);
		void setName(const std::string &str);
		void setParentName(const std::string &str);
		void setIsContext(bool __isContext);
		bool isContext() const;
		bool isDirective() const;
		void appendChild(const Component &comp);
		Component *findFirstChild(std::string __name);
		Component *findFirstChildDirective(std::string __name);
		Component *findFirstChildContext(std::string __name);
		std::vector<Component> findChildren(std::string __name);
		std::vector<Component> findChildrenDirective(std::string __name);
		std::vector<Component> findChildrenContext(std::string __name);
		std::vector<Component> getAllChildrenAndSubChildren();
		std::vector<Component> getAllChildrenAndSubChildren(std::string __name, bool __isContext);
};

#endif
