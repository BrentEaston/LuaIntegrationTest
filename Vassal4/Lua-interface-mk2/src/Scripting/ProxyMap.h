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
	ProxyMap(Map *map);

	ProxyMap(const void *vassalMap);

	virtual ~ProxyMap();

	void registerProxyInfo();

	void setMap (Map *vassalMap);

	Map *getMap();

	void performOperation(const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result);

private:
	Map *vassalMap;

	std::unique_ptr<std::vector<Piece>> visiblePieces;
	std::vector<Piece>::iterator visiblePiecesIterator;
};

#endif /* PROXYMAP_H_ */
