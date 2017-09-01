/*
 * LuaScriptError.cpp
 *
 *  Created on: 31 Aug. 2017
 *      Author: Brent
 */

#include <Lua/lua.hpp>
#include <Scripting/Lua/LuaScriptError.h>
#include <Scripting/ScriptResult.h>
#include <iostream>
#include <sstream>

using namespace std;

// Constructor to build a new error from information provided by the C interface when a problem is detected in the C side of the C-Lua interface
LuaScriptError::LuaScriptError(lua_State *lua, const string source, const string message, const ScriptResult::eResult errorLevel) {
	// cout << "LuaScriptError::LuaScriptError: message=" << message << endl;
	setState(lua);
	setMessage(message);
	setLevel(errorLevel);
}


// Constructor used when an error has passed ia the Lua stack
LuaScriptError::LuaScriptError(lua_State *lua) {
	setState(lua);
}

LuaScriptError::~LuaScriptError() {

}

const string LuaScriptError::formatError() const {
	//return "Source: " + getSource() + "\nMessage: " + getMessage() + "\nInfo: " + getInfo();
	return getInfo();
}

lua_State* &LuaScriptError::getState() {
	return l;
}

void LuaScriptError::setState(lua_State* &state) {
	l = state;
}

void LuaScriptError::setInfo(const string &info) {

	ScriptError::setInfo(info);

	// cout << "LuaScriptError::setInfo: Info set to " << getInfo() << endl;

}

/*
 * Build the error detail using available information
 * Check to see what is on the stack:
 * 0. Nothing on the stack, the error information was passed via the constructor.
 * 1. A single table argument is an existing Lua Error object from an error raised at a deeper level
 * 2. A single string argument is an new unknown error raised from within the Lua run machine
 * 3. 2 arguments, a string and a number is a known error and error level raised within the Lua side of the C-Lua interface
 */
void LuaScriptError::build() {
	int nargs = lua_gettop(l);

	if (nargs == 0) {
		// cout << "LuaScriptError::build No build, error info passed in constructor" << endl;
		setInfo(getStackTrace());
		return;
	}
	else if (nargs == 1 && lua_istable(l, 1)) {
		// cout << "LuaScriptError::build Table on top of stack" << endl;
		buildFromTable();
		return;
	}
	else if (nargs == 1 && lua_isstring(l, 1)) {
		// cout << "LuaScriptError::build String on top of stack=" << lua_tostring(l,1) << endl;
		buildFromString();
		setInfo(getStackTrace());
		return;
	}
	else if (nargs == 2 && lua_isstring(l, 1) && lua_isinteger(l, 2)) {
		// cout << "LuaScriptError::build String and Integer on top of stack=" << lua_tostring(l,1) << ", " << lua_tointeger(l,2) << endl;
		buildFromArguments();
		setInfo(getStackTrace());
		return;
	}
	else {
		// cout << "LuaScriptError::build Don't know what's on top of stack, nargs=" << nargs << endl;
	}

	// Save the stack trace
	// cout << "LuaScriptError::build: Set info to Stack Trace" << endl;
	setInfo(getStackTrace());
}

/*
 * Retrieve and clean up the stack trace from the Lua stack
 * 1. Remove references to C functions (commencing with '[C]')
 * 2. Remove tail calls reference (commencing with '(...tail')
 * 3. Remove references to init.lua ('commencing with 'init.lua')
 * 4. Change [string "ScriptName"] to [ScriptName]
 * 5. If there is no stack trace, (trace ends in '\nstack traceback:'), remove the '\nstack traceback:'
 * 6. Change "in upvalue 'function'" to "in 'function'"
 * 7. Change "in local 'function'" to "in 'function'"
 * 8. Change "in main chunk" to ""
 */
const string LuaScriptError::getStackTrace() const {

	luaL_traceback (l, l, getMessage().c_str(), 2);
	string rawTrace = lua_tostring(l, -1);
	lua_pop(l, 1);

	// Remove Stack trace header text if no actual stack trace
	string tracebackMarker = "\nstack traceback:";
	if (rawTrace.length() > tracebackMarker.length()) {
		if (rawTrace.find(tracebackMarker) == (rawTrace.length() - tracebackMarker.length())) {
			rawTrace = rawTrace.substr(0, (rawTrace.length() - tracebackMarker.length()));
		}
	}

	// Process each line of the trace
	stringstream inTrace (rawTrace);
	stringstream trace;
	string line;
	bool first = true;

	while (getline(inTrace, line, '\n')) {
		if (line.find("\t[C]") == 0) {
			// Skip C functions
		}
		else if (line.find("\t(...tail calls") == 0) {
			// Skip tail calls indicator
		}
		else if (line.find("\tinit.lua") == 0) {
			// Skip references to init.lua
		}
		else {
			//if (line.find(" in main chunk") != string::npos) {
			//	line = line.replace(line.find(" in main chunk"), 14, "in main");
			//}
			if (line.find("in upvalue '") != string::npos) {
				line = line.replace(line.find("in upvalue '"), 12, "in '");
			}
			if (line.find("in local '") != string::npos) {
				line = line.replace(line.find("in local '"), 10, "in '");
			}
			if (line.find("\t[string ") == 0) {
				int pos = line.find("\"]");
				string scriptName = line.substr(10, pos-10);
				line = "[" + scriptName+"]"+line.substr(pos+2,string::npos);
			}

			if (! first) {
				trace << '\n';
			}
			first = false;
			trace << line;
		}

	}

	return trace.str();
}

/*
 * Read error information from the Lua Error object on the top of the Lua stack
 */
void LuaScriptError::buildFromTable() {
	// cout << "LuaScriptError::buildFromTable:" << endl;

	lua_getfield(l, 1, LEVEL_TAG);
	if (lua_isinteger(l, -1)) {
		setLevel((ScriptResult::eResult) lua_tointeger(l, -1));
	}
	lua_pop(l, 1);

	lua_getfield(l, 1, ERROR_TAG);
	if (lua_isstring(l, -1)) {
		setMessage(lua_tostring(l, -1));
	}
	lua_pop(l, 1);

	lua_getfield(l, 1, SOURCE_TAG);
	if (lua_isstring(l, -1)) {
		setSource(lua_tostring(l, -1));
	}
	lua_pop(l, 1);

	lua_getfield(l, 1, INFO_TAG);
	if (lua_isstring(l, -1)) {
		// cout << "LuaScriptError::buildFromTable: Set info to Info in Lua table on stack" << endl;
		setInfo(lua_tostring(l, -1));
	}
	lua_pop(l, 1);
}

void LuaScriptError::buildFromString() {
	// cout << "LuaScriptError::buildFromString:" << endl;
	setMessage(lua_tostring(l,1));
	setLevel(ScriptResult::eResult_Script_Error);
}

void LuaScriptError::buildFromArguments() {
	// cout << "LuaScriptError::buildFromArguments:" << endl;
	setMessage(lua_tostring(l,1));
	setLevel((ScriptResult::eResult) lua_tointeger(l,2));
}


void LuaScriptError::push() const {
	lua_newtable(l);

	lua_pushinteger(l, getLevel());
	lua_setfield(l, -2, LEVEL_TAG);

	lua_pushstring(l,getMessage().c_str());
	lua_setfield(l, -2, ERROR_TAG);

	lua_pushstring(l, getSource().c_str());
	lua_setfield(l, -2, SOURCE_TAG);

	lua_pushstring(l, getInfo().c_str());
	lua_setfield(l, -2, INFO_TAG);
}

void LuaScriptError::throwError() {
	push();
	lua_error(l);

}
