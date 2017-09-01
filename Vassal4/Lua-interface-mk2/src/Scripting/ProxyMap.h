/*
 * ProxyMap.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef PROXYMAP_H_
#define PROXYMAP_H_

#include <Piece.h>
#include <Scripting/Proxy.h>
#include <Scripting/TValue.h>
#include <memory>
#include <string>
#include <vector>

class Map;

using namespace std;

/**
 * A ProxyMap wraps a Vassal Map Game Object and forms the interface between the scripting language and Vassal.
 */
class ProxyMap : public Proxy {
public:
	ProxyMap(Map *map, ContextFrame *frame);

	ProxyMap(const void *vassalMap, ContextFrame *frame);

	virtual ~ProxyMap();

	void registerProxyInfo();

	void setMap (Map *vassalMap);

	Map *getMap() const;

	void performOperation(const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result);

private:
	Map *vassalMap;

};

#endif /* PROXYMAP_H_ */
