/*
 * Map.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <vector>

#include "Piece.h"

class Map: public Scriptable {
public:
	Map(const std::string name);
	Map(Map *map);
	virtual ~Map();

	std::string getName() const;
	void setName(const std::string &myName);

	std::string getProperty(const std::string propertyName) const;

	eType getScriptableType () const;

	std::unique_ptr<std::vector<Piece>> getVisiblePieces();

protected:
	std::string myName;

};

#endif /* MAP_H_ */
