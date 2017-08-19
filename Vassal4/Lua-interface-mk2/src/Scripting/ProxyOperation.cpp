#include <Scripting/ProxyOperation.h>

ProxyOperation::ProxyOperation(std::string name) {
	this -> name = name;
}

ProxyOperation::~ProxyOperation () {

}

const int ProxyOperation::getArgumentCount() const {
	return argTypes.size();
}

const std::string ProxyOperation::getName() const {
	return name;
}

TValue::eType ProxyOperation::getArgumentType(const int argNum) {
	if (argNum < 0 || argNum >= getArgumentCount()) {
		return TValue::eType_invalid;
	}
	else {
		return argTypes[argNum];
	}
}

ProxyOperation *ProxyOperation::addArgument (const TValue::eType type) {
	argTypes.push_back(type);
	return this;
}
