/*
 * ProxyDefinition.cpp
 *
 *  Created on: 12 Aug. 2017
 *      Author: Brent
 */

#include <Scripting/ProxyDefinition.h>
#include <Scripting/ProxyOperation.h>

ProxyDefinition::ProxyDefinition(std::string name, Scriptable::eType type){
	this-> name = name;
	this ->type = type;
}

ProxyDefinition::~ProxyDefinition() {
	// TODO Auto-generated destructor stub
}

const std::string ProxyDefinition::getName() const {
	return name;
}

const Scriptable::eType ProxyDefinition::getType() const {
	return type;
}

std::unique_ptr<ProxyOperation>& ProxyDefinition::registerOperation (const std::string operationName) {

	if (operations.count(operationName) == 0) {
		operations[operationName] = std::make_unique<ProxyOperation> (operationName);
	}
	return operations[operationName];

}

const bool ProxyDefinition::isOperationValid(const std::string operation) const {
	return operations.count(operation) > 0;
}

const int ProxyDefinition::getOperationCount() const {
	return operations.size();
}

std::unique_ptr<ProxyOperation>& ProxyDefinition::getOperation(const std::string operationName) {
	return operations[operationName];
}
