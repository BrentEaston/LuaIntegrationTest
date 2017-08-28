#include <Scripting/Proxy.h>
#include <Scripting/Scriptable.h>

// Initialise static storage
// eType_Invalid, eType_Piece, eType_Map, eType_Face, eType_Location, eType_Surface, eType_Player, eType_Collection
std::string Scriptable::typeNames[] = {"Invalid", "Piece", "Map", "Face", "Location", "Surface", "Player", "Collection" };

Scriptable::~Scriptable() {
	notifyDestructionListeners();
}

void Scriptable::addDestructionListener (Proxy *proxy) {
	destructionListeners.push_back(proxy);
}

void Scriptable::removeDestructionListener (Proxy *proxy) {
	destructionListeners.remove(proxy);
}

void Scriptable::notifyDestructionListeners () {
	 for (std::list<Proxy*>::const_iterator iterator = destructionListeners.begin(), end = destructionListeners.end(); iterator != end; ++iterator) {
	    (*iterator)->notifyDestroyed();
	}
}

void Scriptable::destroy() {
	notifyDestructionListeners();
}
