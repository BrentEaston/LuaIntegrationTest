/*
 * LuaScriptError.h
 *
 *  Created on: 31 Aug. 2017
 *      Author: Brent
 */

#ifndef SCRIPTING_LUA_LUASCRIPTERROR_H_
#define SCRIPTING_LUA_LUASCRIPTERROR_H_

#include <Scripting/ScriptError.h>
#include <string>

struct lua_State;

class LuaScriptError: public ScriptError {
public:

	static constexpr const char *ERROR_TAG = "error";
	static constexpr const char *LEVEL_TAG = "level";
	static constexpr const char *SOURCE_TAG = "source";
	static constexpr const char *INFO_TAG = "info";

	LuaScriptError(lua_State *lua, const string source, const string message, const ScriptResult::eResult errorLevel);
	LuaScriptError(lua_State *lua);

	virtual ~LuaScriptError();

	const string formatError() const;

	void build();
	void throwError();
protected:

	void buildFromTable();
	void buildFromString();
	void buildFromArguments();

	const string getStackTrace() const;

	lua_State* &getState();

	void setState(lua_State* &state);

	void setInfo(const string &info);

	void push() const;
private:
	lua_State *l;
};

#endif /* SCRIPTING_LUA_LUASCRIPTERROR_H_ */
