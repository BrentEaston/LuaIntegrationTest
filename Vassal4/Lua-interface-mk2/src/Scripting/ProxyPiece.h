/*
 * ProxyPiece.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef PROXYPIECE_H_
#define PROXYPIECE_H_

#include <Scripting/Proxy.h>
#include <Scripting/TValue.h>
#include <memory>
#include <string>
#include <vector>

class ScriptResult;

class Piece;

using namespace std;

/**
 * A ProxyPiece wraps a Vassal Piece Game Object and forms the interface between the scripting language and Vassal.
 */
class ProxyPiece : public Proxy {
public:
	ProxyPiece(Piece *piece, ContextFrame *frame);

	ProxyPiece(const void *vassalPiece, ContextFrame *frame);

	virtual ~ProxyPiece();

	void setPiece (Piece *piece);
	Piece *getPiece();

	void registerProxyInfo();

	//bool performOperation(const string operation, vector<unique_ptr<TValue>> &args, unique_ptr<TValue> &result, string &error);
	void performOperation(const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result);

private:
	Piece *vassalPiece;
};

#endif /* PROXYPIECE_H_ */
