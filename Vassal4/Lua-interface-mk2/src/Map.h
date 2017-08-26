/*
 * Map.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef MAP_H_
#define MAP_H_

#include <Piece.h>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class Map: public Scriptable {
public:
	Map(const std::string name);
	Map(Map *map);
	virtual ~Map();

	string getName() const;
	void setName(const std::string &myName);

	TValue *get(const std::string propertyName) const;
	void set (const string id, TValue value);
	TValue *properties() const;

	eType getScriptableType () const;

	std::unique_ptr<std::vector<Piece>> getVisiblePieces();

protected:
	std::string myName;

};

#endif /* MAP_H_ */
