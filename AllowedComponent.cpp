#include "AllowedComponent.hpp"

AllowedComponent::AllowedComponent(std::string __name,
								   bool __isContext,
								   std::vector<std::string> __allowedParents,
								   int __minAttr,
								   int __maxAttr,
								   bool (*_attrIsCorrect)(std::string, int)) : _name(__name),
																			   _isContext(__isContext),
																			   _allowedParents(__allowedParents),
																			   _minAttr(__minAttr),
																			   _maxAttr(__maxAttr),
																			   attrIsCorrect(_attrIsCorrect) {}

AllowedComponent::~AllowedComponent() {}

AllowedComponent::AllowedComponent(const AllowedComponent &ref) : _name(ref._name),
																  _isContext(ref._isContext),
																  _allowedParents(ref._allowedParents),
																  _minAttr(ref._minAttr),
																  _maxAttr(ref._maxAttr),
																  attrIsCorrect(ref.attrIsCorrect) {}

AllowedComponent &AllowedComponent::operator=(const AllowedComponent &ref) {
	_name = ref._name;
	_isContext = ref._isContext;
	_allowedParents = ref._allowedParents;
	_minAttr = ref._minAttr;
	_maxAttr = ref._maxAttr;
	attrIsCorrect = ref.attrIsCorrect;
}

