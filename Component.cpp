#include "Component.hpp"

Component::Component(	std::string __name,
						std::vector<std::string> __attr,
						bool __isContext,
						std::vector<Component> __children
					):	_name(__name),
						_attributes(__attr),
						_isContext(__isContext),
						_children(__children) { }

Component::Component(const Component &ref): _name(ref._name), _attributes(ref._attributes), _isContext(ref._isContext), _children(ref._children) {}

Component &Component::operator=(const Component &ref) {
	_name = ref._name;
	_attributes = ref._attributes;
	_isContext = ref._isContext;
	_children = ref._children;
	return *this;
}

Component::~Component() {}

const std::string& Component::name() const {
	return _name;
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

const std::vector<Component>& Component::children() const {
	return _children;
}

const Component& Component::children(int index) const {
	return _children[index];
}

const std::string& Component::attr(int index) const {
	return _attributes[index];
}

void Component::appendAttr(const std::string &str) {
	_attributes.push_back(str);
}

void Component::appendChild(const Component &comp) {
	_isContext = true;
	_children.push_back(comp);
}

void Component::setName(const std::string &str) {
	_name = str;
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
