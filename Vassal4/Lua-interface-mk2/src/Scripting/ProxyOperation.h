/*
 * ProxyOperation.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef _PROXYOPERATION_H_
#define _PROXYOPERATION_H_

#include <Scripting/TValue.h>
#include <string>
#include <vector>

class ProxyOperation {
public:
	ProxyOperation(std::string name);
	~ProxyOperation();

	const int getArgumentCount() const;
	const std::string getName() const;

	TValue::eType getArgumentType(const int argNum);

	ProxyOperation *addArgument (const TValue::eType type);
private:
	std::string name;
	std::vector <TValue::eType> argTypes;
};

#endif /* _PROXYOPERATION_H_ */
