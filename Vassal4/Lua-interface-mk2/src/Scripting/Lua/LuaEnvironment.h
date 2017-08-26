/*
 * LuaEnvironment.h
 *
 *  Created on: 29/07/2017
 *      Author: Brent
 */

#ifndef LUAENVIRONMENT_H_
#define LUAENVIRONMENT_H_

#include <Scripting/ProxyFactory.h>
#include <Scripting/ScriptingEnvironment.h>
#include <Scripting/ScriptResult.h>
#include <memory>
#include <string>
#include <vector>

class ContextFrame;

struct lua_State;

class LuaEnvironment : public ScriptingEnvironment {

public:

	const std::string INIT_FILE_NAME = "init.lua";

	LuaEnvironment();
	~LuaEnvironment();
	void initialize();

	lua_State* getState();
	void setState(lua_State* state);

	/* Implementation of ScriptingEnvironment */
	void validate (Script *script, ScriptResult &result);
	void execute (Script *script, const Scriptable *context, ScriptResult &result);

	ProxyFactory getProxyFactory() const;

	virtual const bool isInitialised() const;
	virtual const std::string getInitialisationError() const;

	void pushCurrentContext(const Scriptable *context);
	void popCurrentContext();
	std::unique_ptr<ContextFrame> & getCurrentContext() const;
	int getCurrentContextLevel() const;

	// Sandbox limits for a single script, including all sub-scripts
	// TODO Make the configurable Via Vassal Config system
	static const int LUA_STEP_DELTA = 200;			// How often (Lua VM steps) between Memory and CPU checks
	static const long LUA_STEP_LIMIT = 250000; 	// How many Memory/CPU checks before raising an error
	static const int LUA_MEMORY_LIMIT = 500;		// Maximum memory allocated to the Lua Environment
	static const int LUA_STRING_REP_LIMIT = 40;	// Maximum replication argument for string.rep()
	static const int LUA_RE_ENTRY_LIMIT = 40;		// Maximum number of times a script can be called from within another script without returning

	long incrLuaStepCount();
	void resetLuaStepCount();
private:

	void handleScriptError (lua_State *l, ScriptResult &result);

	/** The Global Lua state */
	lua_State *state;

	/** Factory class to create Game Object proxies */
	ProxyFactory proxyFactory ;

	/** Current contextFrame */
	//static ContextFrame currentContextFrame;

	/** Stack of ContextFrames from earlier re-entrant calls*/
	static std::vector<std::unique_ptr<ContextFrame>> contextStack;

	long luaStepCount = 0;						// Lua CPU step counter. How many steps of LUA_STEP_DELTA VM steps have been recorded since a top-level script started running.

};

#endif /* LUAENVIRONMENT_H_ */
