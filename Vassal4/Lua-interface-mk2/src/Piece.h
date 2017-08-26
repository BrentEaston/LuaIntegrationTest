/*
 * Piece.h
 *
 *  Created on: 15/07/2017
 *      Author: Brent
 */

#ifndef PIECE_H_
#define PIECE_H_

#include <Scripting/Scriptable.h>
#include <memory>
#include <string>

class Map;

class Piece : public Scriptable {
public:
	Piece();
	Piece(const std::string, const int level);
	Piece(Piece *piece);
	virtual ~Piece();

	std::string toString() const;
	int getLevel() const;
	void setLevel(const int myLevel);
	std::string getName() const;
	void setName(const std::string &myName);

	std::string get(const std::string propertyName) const;

	eType getScriptableType () const;

	void setMap (Map *map);
	const Map *getMap();

protected:
	std::string myName;
	int myLevel;
	Map *myMap;

};

#endif /* PIECE_H_ */
