/*
 * Scriptable.h
 *
 *  Created on: 08/08/2017
 *      Author: Brent
 */

#ifndef SCRIPTABLE_H_
#define SCRIPTABLE_H_

#include <PropertyContainer.h>
#include <list>
#include <string>

class Proxy;


/**
 * Scriptable is an interface that should be inherited by any Vassal Game Object that can be interacted with
 * by Scripts
 *
 */

class Scriptable: public PropertyContainer {
public:

	/**
	 * Enumerate the different types of Vassal Game Objects that can be scriptable.
	 * Each Scriptable returns its type (see getScriptableType).
	 * This is required for the scripting language interface.
	 * Any changes here must be reflected in init.lua
	 */
	enum eType {
		eType_Invalid, eType_Piece, eType_Map, eType_Face, eType_Location, eType_Surface, eType_Player, eType_Collection
	};

	static const eType eType_FIRST = eType_Piece;
	static const eType eType_LAST = eType_Collection;

	static std::string typeNames[];

	static std::string getTypeName(eType type) {

		if ((type >= eType_FIRST && type <= eType_LAST) || type == eType_Invalid) {
			return typeNames[type];
		}
		return "Unknown";
	}

	static bool isValidType(eType type) {
		return type >= eType_FIRST && type <= eType_LAST;
	}

	virtual ~Scriptable();

	/**
	 * Return the type of a Scriptable.
	 */
	virtual eType getScriptableType() const=0;

	/*
	 *
	 */
	virtual void addDestructionListener (Proxy *proxy);
	virtual void removeDestructionListener (Proxy *proxy);
	virtual void notifyDestructionListeners ();
	virtual void destroy();

private:

	list<Proxy*> destructionListeners;
};

#endif /* SCRIPTABLE_H_ */
