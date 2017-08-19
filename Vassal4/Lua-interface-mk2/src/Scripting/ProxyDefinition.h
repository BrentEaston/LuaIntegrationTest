/*
 * ProxyDefinition.h
 *
 *  Created on: 12 Aug. 2017
 *      Author: Brent
 */

#ifndef _PROXYDEFINITION_H_
#define _PROXYDEFINITION_H_

#include <Scripting/Scriptable.h>
#include <memory>
#include <string>
#include <unordered_map>

class ProxyOperation;

class ProxyDefinition {
public:
	ProxyDefinition(std::string name, Scriptable::eType type);
	virtual ~ProxyDefinition();

	std::unique_ptr<ProxyOperation>& registerOperation (const std::string operationName);

	const std::string getName() const;
	const Scriptable::eType getType() const;

	const bool isOperationValid(const std::string operation) const;
	const int getOperationCount() const;

	std::unique_ptr<ProxyOperation>& getOperation(const std::string operationName);
private:

		std::string name;
		Scriptable::eType type;
		std::unordered_map<std::string, std::unique_ptr<ProxyOperation>> operations;
};

#endif /* _PROXYDEFINITION_H_ */
