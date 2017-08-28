/*
 * ProxyCollection.cpp
 *
 *  Created on: 27 Aug. 2017
 *      Author: Brent
 */

#include <Collection.h>
#include <Scripting/ProxyCollection.h>
#include <Scripting/ProxyDefinition.h>
#include <Scripting/Scriptable.h>
#include <Scripting/ScriptResult.h>
#include <algorithm>
#include <iostream>

#define PROXY_NAME "Collection"
#define PROXY_TYPE Scriptable::eType_Collection

using namespace std;

ProxyCollection::ProxyCollection(Collection *collection, ContextFrame *frame) {
	// cout << "ProxyCollection::ProxyCollection 1" << endl;
	registerProxyInfo();
	vassalCollection = collection;
	setFrame(frame);
}

ProxyCollection::ProxyCollection(const void *collection, ContextFrame *frame) {
	// cout << "ProxyCollection::ProxyCollection 2" << endl;
	registerProxyInfo();
	vassalCollection = (Collection *) collection;
	setFrame(frame);
}

ProxyCollection::~ProxyCollection() {
	// cout << "ProxyCollection::~ProxyCollection" << endl;
}

void ProxyCollection::registerProxyInfo() {
	if (!isRegistered(PROXY_NAME)) {
		auto def = make_unique<ProxyDefinition> (PROXY_NAME, PROXY_TYPE);

		// collection.next() return the next element of the Collection, or nil if eof
		def -> registerOperation("next");

		registerProxy(def);

	}
}

void ProxyCollection::performOperation(const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result) {
	// cout << "ProxyCollection::performOperation: operation = [" << operation << "]" << endl;

	if (operation == "next") {
		if (nextPtr >= vassalCollection->size()) {
			// cout << "ProxyCollection::performOperation:next Iteration complete, return nil" << endl;
			result.setResultValue(make_unique<TValue>());
		}
		else {
			Scriptable *item = vassalCollection->getItem(nextPtr++);
			// cout << "ProxyCollection::performOperation:next next item, type=" << item->getScriptableType() << ", ptr=" << item << endl;
			result.setResultValue(make_unique<TValue>(item));
		}
	}
}

Collection *ProxyCollection::getCollection() const {
	return vassalCollection;
}
