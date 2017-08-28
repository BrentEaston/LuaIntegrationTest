/*
 * ProxyCollection.h
 *
 *  Created on: 27 Aug. 2017
 *      Author: Brent
 */

#ifndef SCRIPTING_PROXYCOLLECTION_H_
#define SCRIPTING_PROXYCOLLECTION_H_

#include <Scripting/Proxy.h>
#include <Scripting/TValue.h>
#include <memory>
#include <string>
#include <vector>

class ContextFrame;

class Collection;

class ScriptResult;

using namespace std;

class ProxyCollection : public Proxy {
public:
	ProxyCollection(Collection *collection, ContextFrame *frame);
	ProxyCollection(const void *collection, ContextFrame *frame);

	virtual ~ProxyCollection();

	void registerProxyInfo();
	void performOperation(const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result);

	Collection *getCollection() const;

private:

	unique_ptr<Collection> vassalCollection;
	int nextPtr = 0;


};

#endif /* SCRIPTING_PROXYCOLLECTION_H_ */
