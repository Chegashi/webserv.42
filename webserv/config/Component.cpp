#include "Component.hpp"

Component::Component(	std::string __name, std::string __parentName,
						std::vector<std::string> __attr,
						bool __isContext,
						std::vector<Component> __children,
						int __line,
						int __col,
						int __depth
					):	_name(__name), _parentName(__parentName),
						_attributes(__attr),
						_isContext(__isContext),
						_children(__children),
						_line(__line),
						_col(__col),
						_depth(__depth) { }

Component::Component(const Component &ref): _name(ref._name), _parentName(ref._parentName),
											_attributes(ref._attributes),
											_isContext(ref._isContext),
											_children(ref._children),
											_line(ref._line),
											_col(ref._col),
											_depth(ref._depth) {}

Component &Component::operator=(const Component &ref) {
	_name = ref._name;
	_parentName = ref._parentName;
	_attributes = ref._attributes;
	_isContext = ref._isContext;
	_children = ref._children;
	_line = ref._line;
	_col = ref._col;
	_depth = ref._depth;
	return *this;
}

Component::~Component() {}

const std::string& Component::name() const {
	return _name;
}

const std::string& Component::parentName() const {
	return _parentName;
}

bool Component::isContext() const {
	return _isContext;
}

bool Component::isDirective() const {
	return !_isContext;
}

const std::vector<std::string>& Component::attr() const {
	return _attributes;
}

std::vector<Component>& Component::children() {
	return _children;
}

Component& Component::children(int index) {
	return _children[index];
}

const std::string& Component::attr(int index) const {
	return _attributes[index];
}

void Component::appendAttr(const std::string &str) {
	_attributes.push_back(str);
}

void Component::removeChild(std::vector<Component>::iterator it) {
	_children.erase(it);
}

void Component::appendChild(const Component &comp) {
	_isContext = true;
	_children.push_back(comp);
}

void Component::setName(const std::string &str) {
	_name = str;
}

void Component::setParentName(const std::string &str) {
	_parentName = str;
}

int Component::line() const {
	return _line;
}

int Component::col() const {
	return _col;
}

void Component::setLine(int __line) {
	_line = __line;
}

void Component::setCol(int __col) {
	_col = __col;
}

int Component::depth() const {
	return _depth;	
}

void Component::setDepth(int __depth) {
	_depth = __depth;	
}

void Component::setIsContext(bool __isContext) {
	_isContext = __isContext;
}


Component *Component::findFirstChild(std::string __name) {
	for (std::vector<Component>::iterator it = _children.begin(); it != _children.end(); it++) {
		if (it->_name == __name) {
			return &(*it);
		}
	}
	return NULL;
}


Component *Component::findFirstChildDirective(std::string __name) {
	for (std::vector<Component>::iterator it = _children.begin(); it != _children.end(); it++) {
		if (it->_name == __name && it->isDirective()) {
			return &(*it);
		}
	}
	return NULL;
}

Component *Component::findFirstChildContext(std::string __name) {
	for (std::vector<Component>::iterator it = _children.begin(); it != _children.end(); it++) {
		if (it->_name == __name && it->isContext()) {
			return &(*it);
		}
	}
	return NULL;
}

std::vector<Component> Component::findChildren(std::string __name) {
	std::vector<Component> ret;
	for (std::vector<Component>::iterator it = _children.begin(); it != _children.end(); it++) {
		if (it->name() == __name) {
			ret.push_back(*it);
		}
	}
	return ret;
}

std::vector<Component> Component::findChildrenDirective(std::string __name) {
	std::vector<Component> ret;
	for (std::vector<Component>::iterator it = _children.begin(); it != _children.end(); it++) {
		if (it->name() == __name && it->isDirective()) {
			ret.push_back(*it);
		}
	}
	return ret;
}

std::vector<Component> Component::findChildrenContext(std::string __name) {
	std::vector<Component> ret;
	for (std::vector<Component>::iterator it = _children.begin(); it != _children.end(); it++) {
		if (it->name() == __name && it->isContext()) {
			ret.push_back(*it);
		}
	}
	return ret;
}

std::vector<Component> Component::getAllChildrenAndSubChildren() {
	std::vector<Component> ret;

	for (std::vector<Component>::iterator it = _children.begin(); it != _children.end(); it++) {
		ret.push_back(Component(it->name(), it->parentName(), it->attr(), it->isContext(), std::vector<Component>(), it->line(), it->col(), it->depth()));
		if (it->isContext()) {
			std::vector<Component> tmp = it->getAllChildrenAndSubChildren();
			ret.insert(ret.end(), tmp.begin(), tmp.end());
		}
	}
	return ret;
}


std::vector<Component> Component::getAllChildrenAndSubChildren(std::string __name, bool __isContext) {
	std::vector<Component> ret;

	for (std::vector<Component>::iterator it = _children.begin(); it != _children.end(); it++) {
		if (it->name() == __name && it->isContext() == __isContext) {
			ret.push_back(Component(it->name(), it->parentName(), it->attr(), it->isContext(), std::vector<Component>(), it->line(), it->col(), it->depth()));
			if (it->isContext()) {
				std::vector<Component> tmp = it->getAllChildrenAndSubChildren(__name, __isContext);
				ret.insert(ret.end(), tmp.begin(), tmp.end());
			}
		}
	}
	return ret;
}
