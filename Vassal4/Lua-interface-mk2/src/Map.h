/*
 * Map.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef MAP_H_
#define MAP_H_

#include <Scripting/Scriptable.h>
#include <Scripting/TValue.h>
#include <memory>
#include <string>

class Collection;

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

	Collection *getVisiblePieces();

protected:
	std::string myName;
	// TODO Where should Collection objects live? Can't really be here as it is not re-entrant.
	std::unique_ptr<Collection> visiblePieces;

};

#endif /* MAP_H_ */
