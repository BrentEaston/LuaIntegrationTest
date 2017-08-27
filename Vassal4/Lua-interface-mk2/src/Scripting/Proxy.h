/*
 * Proxy.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef _PROXY_H_
#define _PROXY_H_

#include <Scripting/ProxyDefinition.h>
#include <Scripting/Scriptable.h>
#include <Scripting/TValue.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class ScriptResult;

class ProxyDefinition;
class ProxyOperation;
class TValue;
class Scriptable;

// To Create a new Proxy type, the following needs to be updated:
// 1. Update Scriptable.h with a new Scriptable type. The Vassal Object code must implement Scriptable
// 2. Implement a new ProxyXXXXX class, subclassing Proxy to wrap the new Scriptable type. Implement
//    ProxyXXXXX.registerProxyInfo and ProxyXXXXX.performOperation
// 3. Implement a new XXXXX Lua object and operation functions in init.lua

// To Create a new Operation for an existing Proxy, the following needs to be updated:
// 1. Update ProxyXXXXX::registerProxyInfo with the name and argument types of the operation
// 2. Update ProxyXXXXX::performOperation with the code to implement the new operation
// 3. Update the XXXXX Lua Object definition in init.lua to add a new function call to support the operation

// TODO Need to be able tell if the corresponding Vassal object has been destroyed (e.g. GamePiece deleted)
// When a derived Proxy class is created, it needs to register with it's contained Vassal object as a listener.
// When a Vassal object is destroyed, it must notify all listeners that it has been destroyed.

using namespace std;

class Proxy {
public:
	virtual ~Proxy();
	virtual void performOperation(const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result)=0;

	void notifyDestroyed();

protected:

	static unordered_map<string, unique_ptr<ProxyDefinition>> proxyDefinitions;

	virtual void registerProxyInfo()=0;
	const string getProxyName();
	const Scriptable::eType getProxyType();

	unique_ptr<ProxyOperation> registerOperation (const string operationName);
	bool isRegistered(const string proxyName);
	void registerProxy(unique_ptr<ProxyDefinition> &proxyDefinition);

	void checkOperationArguments (const string proxyName, const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result);

	bool isDestroyed() const;
	bool destroyed = false;
};

#endif /* _PROXY_H_ */
