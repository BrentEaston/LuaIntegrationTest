/*
 * Piece.h
 *
 *  Created on: 15/07/2017
 *      Author: Brent
 */

#ifndef PIECE_H_
#define PIECE_H_

#include <Scripting/Scriptable.h>
#include <Scripting/TValue.h>
#include <string>

using namespace std;

class Map;

class Piece : public Scriptable {
public:
	Piece();
	Piece(const string, const int level);
	Piece(Piece *piece);
	virtual ~Piece();

	string toString() const;
	int getLevel() const;
	void setLevel(const int myLevel);
	string getName() const;
	void setName(const string &myName);

	TValue *get(const string id) const;
	void set (const string id, TValue value);
	TValue *properties() const;

	eType getScriptableType () const;

	void setMap (Map *map);
	const Map *getMap();

protected:
	string myName;
	int myLevel;
	Map *myMap;

};

#endif /* PIECE_H_ */
