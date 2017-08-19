/*
 * ScriptingFactory.cpp
 *
 *  Created on: 01/08/2017
 *      Author: Brent
 *
 * A singleton that creates and returns the correct type of Scripting Environment, depending
 * on the language selected.
 */

#include <Scripting/Lua/LuaEnvironment.h>
#include <Scripting/ScriptingFactory.h>

ScriptingFactory *ScriptingFactory::instance = nullptr;

ScriptingFactory::ScriptingFactory() {
	language = ScriptingFactory::eLua;	// Only supported language atm
	environment = nullptr;
}


/* ---------------- Return the Singleton -------------------- */
ScriptingFactory* ScriptingFactory::getInstance() {
	if (instance == nullptr) {
		instance = new ScriptingFactory();
	}
	return instance;
}

std::unique_ptr<ScriptingEnvironment>& ScriptingFactory::getEnvironment() {
	if (environment == nullptr) {
		switch (language) {
		case eLua:
			environment = std::make_unique<LuaEnvironment>();
			break;
		case eJavascript:
			break;
		case ePython:
			break;
		}
	}
	return environment;
}

