/*
 * LuaError.h
 *
 *  Created on: 24 Aug. 2017
 *      Author: Brent
 */

#ifndef SCRIPTING_LUA_LUAERROR_H_
#define SCRIPTING_LUA_LUAERROR_H_

#include <Lua/lua.hpp>
#include <Scripting/ScriptResult.h>
#include <string>

using namespace std;

class LuaError {
public:
	static constexpr const char  *ERROR_TAG = "error";
	static constexpr const char  *LEVEL_TAG = "level";
	static constexpr const char  *SCRIPT_TAG = "script";
	static constexpr const char  *LINE_TAG = "line";

	static const int STACK_LOOKBACK_LEVEL = 2;

	LuaError(lua_State *l);
	virtual ~LuaError();

	void build();
	void throwError();

	string formatError() const;
	ScriptResult::eResult getErrorLevel() const;
private:
	void extractSourceFromString (const string errorMessage);
	void extractSourceFromLua (const string errorMessage);
	void extractSourceFromError (const int stackPos);
	string extractSourceName (const char nameString[]);

	lua_State *l;
	int stackLevel;
	ScriptResult::eResult errorLevel;
	string errorMessage;
	string sourceName;
	int sourceLine;

};

#endif /* SCRIPTING_LUA_LUAERROR_H_ */
