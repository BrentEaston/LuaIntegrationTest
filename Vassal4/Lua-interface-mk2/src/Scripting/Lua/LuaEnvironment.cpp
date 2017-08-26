/*
 * LuaEnvironment.cpp
 *
 * A LuaEnvironment is a sandboxes Lua interpreter
 *
 *
 *  Created on: 29/07/2017
 *      Author: Brent
 *
 */

#include <Lua/lua.hpp>
#include <Scripting/ContextFrame.h>
#include <Scripting/Lua/LuaEnvironment.h>
#include <Scripting/Lua/LuaError.h>
#include <Scripting/Proxy.h>
#include <Scripting/Script.h>
#include <Scripting/Scriptable.h>
#include <Scripting/TValue.h>
#include <Scripting/ScriptResult.h>
#include <iostream>

using namespace std;

/*
 * Allocate storage for the globals
 */
// ContextFrame LuaEnvironment::currentContextFrame();
vector<unique_ptr<ContextFrame>> LuaEnvironment::contextStack;

/*
 * Callback routines from Lua back into our c++ environment MUST have the function
 * signature
 * 	  static int xxxxxx (lua_State *l)
 * and so we need some global variables within this file to communicate between the static
 * callback functions and our dynamic c++ objects.
 */

static LuaEnvironment *luaEnvironment;

/**
 * Handle an error detected during execution of a callback by pushing an error message on to the stack
 * and calling lua_error to jump back to the last pcall.
 *
 * Needs to be static to be callable from callback and Lua
 */
extern "C" {

void cbAssert(lua_State *l, const bool test, const string source, const string message, const ScriptResult::eResult errorLevel) {

	if (!test) {
		// Create an error object on the Stack and call lua_error
		// cout << "cbAssert: Error raised=" << message << endl;
		lua_newtable(l);
		lua_pushinteger(l, errorLevel);
		lua_setfield(l, -2, "level");
		string error = (source.length() == 0) ? message : source + ": " + message;
		lua_pushstring(l, error.c_str());
		lua_setfield(l, -2, "error");
		lua_error(l);
	}
}

}

/*
 * Handle errors raised in the Lua part of the interface. Errors from deeper levels of call will propagate back up
 * to the top-level script
 *
 * The C interface gives us more access to the Lua internals to generate a better error message including the
 * line number of the script.
 *
 * raiseError is called with 3 arguments pushed on the stack:
 * 1. An Error object that may be a String, or an Error table
 * 2. An Error level, Script or Vassal
 * 3. An indicator of how many Lua stack frames back the error was generated
 *
 * Read the arguments, convert a String error to an error table and call lua_error
 */
static int raiseError(lua_State *l) {
	// cout << "raiseError: In raiseError" << endl;

	LuaError error (l);
	error.build();
	// cout << "raiseError: Error=" << error.formatError() << endl;
	error.throwError();

	return 0;
}
/**
 * Static callback routine used by Lua scripts to act on Vassal game objects.
 *
 * All action on Vassal Game Objects is processed through an intermediate Proxy object. User scripts do not call Vassal Game Objects
 * directly.
 *
 * Arguments pushed on to the Lua stack for a call to callback are:
 * 1. The type of the Vassal Game Object Proxy (Scriptable::eType)
 * 2. A pointer to the C++ Vassal Game Object Proxy
 * 3. A pointer to the C++ ContextFrame object for this script
 * 4. The name of the operation to perform on the Vassal Game Object (See registerProxyInfo of the matching ProxyXXXXX object)
 * 5  onwards. 0 or more parameters required by the operation. These parameters may be a string, integer,
 *    number, boolean, a Vassal Game Object, or an Iterator. A Vassal Game Object or Iteratoris referenced by 3 parameters:
 *    1. a pointer to the Vassal Game Object or Iterator
 *    2. The Type of the parameter (Object or Iterator) (TValue::etype)
 *    3. The type of the object or Iterator (Scriptable::eType)
 *
 * All Proxy Objects for this Script invocation are stored in the ContextFrame pointed to by argument 3.
 */
static int callback(lua_State *l) {

	int nargs = lua_gettop(l);

	// cout << "callback: nargs=" << nargs << endl;

	string id = "Callback";

	// 1st argument must be an integer and must represent a valid Scriptable type
	cbAssert(l, nargs > 0, id, "[1] No arguments supplied in Callback", ScriptResult::eResult_Vassal_Error);
	cbAssert(l, lua_isinteger(l, 1), id, "Callback arg 1 is not an integer", ScriptResult::eResult_Vassal_Error);
	Scriptable::eType contextType = (Scriptable::eType) lua_tointeger(l, 1);
	id += ":" + Scriptable::getTypeName(contextType);
	cbAssert(l, Scriptable::isValidType(contextType), id, "Callback arg 1 type is not valid (" + to_string(contextType) + ")",
			ScriptResult::eResult_Vassal_Error);
	// cout << "Callback: Target type =" << contextType << endl;

	// 2nd argument must be a pointer to the Scriptable to use as a context
	cbAssert(l, nargs > 1, id, "Callback arg 2 missing", ScriptResult::eResult_Vassal_Error);
	cbAssert(l, lua_islightuserdata(l, 2), id, "Callback arg 2 is not a pointer", ScriptResult::eResult_Vassal_Error);
	const void *contextPtr = lua_topointer(l, 2);
	// cout << "callback: Target object passed to callback=" << contextPtr << endl;

	// 3rd Argument must be a raw pointer to the current ContextFrame
	cbAssert(l, nargs > 2, id, "Callback arg 3 missing", ScriptResult::eResult_Vassal_Error);
	cbAssert(l, lua_islightuserdata(l, 3), id, "Callback arg 3 is not a pointer", ScriptResult::eResult_Vassal_Error);
	ContextFrame *myContext = (ContextFrame *) lua_topointer(l, 3);
	// cout << "callback: Context passed to callback=" << myContext << endl;

	// 4th Argument must be the name of the operation to perform on the Scriptable
	cbAssert(l, nargs > 3, id, "Callback arg 4 missing", ScriptResult::eResult_Vassal_Error);
	cbAssert(l, lua_isstring(l, 4), id, "Callback arg 4 is not a string", ScriptResult::eResult_Vassal_Error);
	string operation = lua_tostring(l, 4);
	id += ":" + operation + "[";

	// Convert the arguments to TValue's to pass to the proxy
	// Arguments 4 onwards are the arguments for the request operation.
	vector<unique_ptr<TValue>> args;
	int actualParamIdx = 5;
	while (actualParamIdx <= nargs) {

		if (lua_isstring(l, actualParamIdx)) {
			string s = lua_tostring(l, actualParamIdx++);
			id += ":" + s;
			// cout << "Callback: op arg " << ++opParamIdx << " String=" << s << endl;
			// cout << "Callback: Arg count is " << args.size() << endl;
			args.push_back(make_unique<TValue>(s));
			// cout << "Callback: Arg count is " << args.size() << ", arg 1 type is " << args.front()->getType() << endl;

		} else if (lua_isinteger(l, actualParamIdx)) {
			int i = lua_tointeger(l, actualParamIdx++);
			id += ":" + to_string(i);
			// cout << "Callback: op arg " << ++opParamIdx << " Integer=" << i << endl;
			args.push_back(make_unique<TValue>(i));

		} else if (lua_isnumber(l, actualParamIdx)) {
			float d = lua_tonumber(l, actualParamIdx++);
			id += ":" + to_string(d);
			// cout << "Callback: op arg " << ++opParamIdx << " Float=" << d << endl;
			args.push_back(make_unique<TValue>(d));

		} else if (lua_isboolean(l, actualParamIdx)) {
			bool b = lua_toboolean(l, actualParamIdx++);
			id += ":" + to_string(b);
			// cout << "Callback: op arg " << ++opParamIdx << " Boolean=" << b << endl;
			args.push_back(make_unique<TValue>(b));

		} else if (lua_islightuserdata(l, actualParamIdx)) {
			string paramId = ":Object";
			// A lightuserdata is a pointer to a Vassal Game Object or Iterator.
			const void * pointerArg = lua_topointer(l, actualParamIdx++);
			cbAssert(l, actualParamIdx < nargs, id + paramId + "]", "Object arg not followed by parameter type arg",
					ScriptResult::eResult_Vassal_Error);

			// The Parameter type MUST be followed by the Object type
			cbAssert(l, actualParamIdx < nargs, id + paramId + "]", " Object arg not followed by object type argument",
					ScriptResult::eResult_Vassal_Error);
			cbAssert(l, lua_isinteger(l, actualParamIdx), id + paramId + "]",
					" Object arg not followed by integer arg for Object type", ScriptResult::eResult_Vassal_Error);
			Scriptable::eType objectType = (Scriptable::eType) lua_tointeger(l, actualParamIdx++);

			cbAssert(l, Scriptable::isValidType(objectType), id + paramId + "]",
					"Object type is not valid (" + to_string(objectType) + ")", ScriptResult::eResult_Vassal_Error);

			// TODO Work out why the hell I get a const correctness error message when calling
			// 		TValue(pointerArg, objectType)
			// but not when calling the 1 argument version
			// 		TValue(pointerArg)
			// Lua returns the pointer as const void *, can't change that
			unique_ptr<TValue> tv = make_unique<TValue>(pointerArg);
			tv->setObjectType(objectType);

			// Create the TValue and add it to the Argument list
			args.push_back(move(tv));

			// cout << "Callback: op arg " << ++opParamIdx << " " << ptype << " type"  << endl;

			// Add the parameter id to the callback description string.
			id += paramId;

		} else {
			cbAssert(l, 0, id + "]", "Unknown Argument type from Lua", ScriptResult::eResult_Vassal_Error);
		}
	}

	id += "]";

	// cout << "Callback:  - " << id << endl;

	// Ask the Proxy to perform the operation. It will take care of 'talking' to the real Vassal Game Object
	// performOperation returns false if there was a Vassal-caused error in the Lua <> Vassal protocol and returns
	// the error message in error.
	// If performOperation returns true, then there was no Vassal-caused error, but there may still be some script
	// related error that will be returned to the calling script in the result.

	ScriptResult scriptResult;

	myContext->getProxy(contextType, contextPtr)->performOperation(operation, args, scriptResult);

	/*
	 *  Pop the args that where sent to us from Lua off the stack.
	 *  NOTE: These cannot be popped prior to this as their value becomes undefined once popped
	 */

	lua_pop(l, nargs);

	// If there is an error trying to talk to the Vassal object, abort!
	cbAssert(l, scriptResult.getResultLevel() == ScriptResult::eResult_Success, id, scriptResult.getError(),
			scriptResult.getResultLevel());

	int returnArgsCount = 0;
	TValue result = *(scriptResult.getResult().get());

	// cout << "Callback: result type was " << result.getType() << endl;
	// cout << "Callback: result type2 was " << scriptResult.getResult()->getType() << endl;
	// Convert the result from the TValue back to a Lua value and push it on to the stack
	switch (scriptResult.getResult()->getType()) {
	case TValue::eType_integer:
		lua_pushinteger(l, result.getIntegerValue());
		returnArgsCount++;
		break;
	case TValue::eType_float:
		lua_pushnumber(l, result.getFloatValue());
		returnArgsCount++;
		break;
	case TValue::eType_string:
		lua_pushstring(l, result.getStringValue().c_str());
		returnArgsCount++;
		break;
	case TValue::eType_boolean:
		lua_pushboolean(l, result.getBooleanValue());
		returnArgsCount++;
		break;
	case TValue::eType_Vobject:
		lua_pushlightuserdata(l, result.getObjectPointer());
		returnArgsCount++;
		lua_pushinteger(l, result.getObjectType());
		returnArgsCount++;
		break;
	case TValue::eType_nil:
		lua_pushnil(l);
		returnArgsCount++;
		break;
	default:
		cbAssert(l, 0, id, "TValue from Vassal with unknown type (" + to_string(result.getType()) + ")",
				ScriptResult::eResult_Vassal_Error); /* Unknown TValue type */
		break;
	}

	return returnArgsCount;
}

/*
 * Debug hook called from within Lua every 200 (configuable) VM steps executed.
 * Check for memory and CPU over-usage.
 */
static void hook(lua_State* l, lua_Debug *ar) {
	cbAssert(l, (luaEnvironment->incrLuaStepCount() <= LuaEnvironment::LUA_STEP_LIMIT), "", "Script using too much CPU",
			ScriptResult::eResult_Script_Error);
	cbAssert(l, (lua_gc(l, LUA_GCCOUNT, 0) <= LuaEnvironment::LUA_MEMORY_LIMIT), "", "Script using too much Memory",
			ScriptResult::eResult_Script_Error);
}

/*
 * Create and initialise a Lua scripting environment
 */
LuaEnvironment::LuaEnvironment() {
	initialised = false;
	initialisationError = "";

	setState(nullptr);
	initialize();
	luaEnvironment = this;
}

LuaEnvironment::~LuaEnvironment() {
	if (state != nullptr) {
		lua_close(state);
	}
}

/*
 * Lazy initialisation of the Lua Environment.
 */
void LuaEnvironment::initialize() {

	// Create an empty context frame
	// currentContextFrame = make_unique<ContextFrame> ();

	// Create the Lua Environment first time through
	if (getState() == nullptr) {
		// Create a new Lua state
		lua_State* l = luaL_newstate();
		setState(l);

		// Open all libraries for now.
		luaL_openlibs(state);

		// Load the Lua initialisation file
		if (luaL_loadfile(getState(), INIT_FILE_NAME.c_str())) {
			string error = lua_tostring(l, -1);
			lua_pop(l, 1);
			initialisationError = "Lua Environment failed to initialise. Error compiling " + INIT_FILE_NAME + ": " + error;
			return;
		}

		if (lua_pcall(l, 0, LUA_MULTRET, 0)) {
			string error = lua_tostring(l, -1);
			lua_pop(l, 1);
			initialisationError = "Lua Environment failed to initialise. Error running " + INIT_FILE_NAME + ": " + error;
			return;
		}

		// Register the callback function
		lua_register(l, "callback", callback);

		// Register the error handler
		lua_register(l, "raiseError", raiseError);

		// Register the cpu/memory check hook
		lua_sethook(l, hook, LUA_MASKCOUNT, LuaEnvironment::LUA_STEP_DELTA);

		// Mark enviromnent as successfully initialised
		initialised = true;
	}
}

void LuaEnvironment::setState(lua_State* state) {
	this->state = state;
}

lua_State* LuaEnvironment::getState() {
	return this->state;
}

const bool LuaEnvironment::isInitialised() const {
	return initialised;
}

/*!
 * \brief Has the Scripting Environment been successfully initialised?
 */
const string LuaEnvironment::getInitialisationError() const {
	return initialisationError;
}

/*
 * Translate an Error object returned from Lua into a ScriptResult.
 */
void LuaEnvironment::handleScriptError(lua_State *l, ScriptResult &result) {

	// cout << "LuaEnvironment::handleScriptError: context level=" << getCurrentContextLevel() << endl;
	// Get error details off stack
	LuaError error (l);
	error.build();

	// Back to the top level script?
	if (getCurrentContextLevel() == 1) {
		// Yes, Set the error details into the result
		result.setError(error.formatError(), error.getErrorLevel());
	}
	else {
		// No, Pass error up to the next level
		popCurrentContext();
		error.throwError();
	}

}

void LuaEnvironment::validate(Script *expression, ScriptResult &result) {
	// cout << "LuaEnvironment::validate start, top=" << lua_gettop(getState()) << endl;
	lua_State *l = getState();

	// Get Lua to compile the expression (this does not run it)
	int i = luaL_loadstring(l, expression->getExecutableSource().c_str());

	// Invalid, set the error into the expression, pop the error off the Lua stack and return
	if (i) {
		string err = lua_tostring(l, -1);
		int pos = err.find(']', 0);
		result.setScriptError("["+expression->getName()+err.substr(pos, string::npos));
		lua_pop(l, lua_gettop(l));
		return;
	}

	// No error, clear the error flag in the expression and pop the compiled expression off the Lua stack.
	result.clearError();
	lua_pop(l, lua_gettop(l));
	return;
}

void LuaEnvironment::execute(Script *script, const Scriptable *thisPtr, ScriptResult &result) {
	lua_State *l = getState();

	// cout << "LuaEnvironment::execute start, top=" << lua_gettop(getState()) << endl;

	//
	if (getCurrentContextLevel() >= LUA_RE_ENTRY_LIMIT) {
		result.setScriptError("Scripts nested too deeply");
		return;
	}

	// Create a new ContextFrame containing the 'this' proxy on to the top of the Context Stack
	pushCurrentContext(thisPtr);

	// cout << "LuaEnvironment::execute before getglobal, top=" << lua_gettop(getState()) << endl;

	// Load the sandbox code executor _M.eval
	lua_getglobal(l, "_M");
	lua_getfield(l, -1, "eval");
	lua_remove(l, -2);

	// cout << "LuaEnvironment::execute after getfield, top=" << lua_gettop(getState()) << endl;

	// And push the required arguments
	// The Parameter sequence for calling _M.eval is
	// 1. The source of the script to be executed
	// 2. The name of the script
	// 3. The Scriptable::eType of the Vassal Object that the script is running against
	// 4. A (void *) ptr to the to the Vassal Object.
	// 5. A (void *) ptr to the current ContextFrame containing all Proxy Objects used in this script invocation
	//
	lua_pushstring(l, script->getExecutableSource().c_str());	// Script code
	lua_pushstring(l, script->getName().c_str());				// Script name
	lua_pushinteger(l, thisPtr->getScriptableType());	 		// Context type
	lua_pushlightuserdata(l, ((void *) thisPtr));    		// Context reference
	lua_pushlightuserdata(l, ((void *) getCurrentContext().get())); // ContextFrame reference

	// cout << "LuaEnvironment::execute before pcall, script: " << script->getName() << endl;
	// Call the compiled code.
	//
	// lua_pcall will return 0 if the sandbox successfully attempts to execute the script (even if the script itself fails)
	// A non-zero return indicates the sandbox evaluation routine _M.eval failed.
	// 5 Arguments are passed to _M.eval and 2 returned
	if (lua_pcall(l, 5, 2, 0) != LUA_OK) {
		// cout << "LuaEnvironment::execute pcall failed pcall, script: " << script->getName() << endl;

		handleScriptError (l, result);

	} else {
		// cout << "LuaEnvironment::execute pcall succeeded, script: " << script->getName() << endl;

		// cout << "Stack top type = " << lua_typename(l, lua_type(l, -1)) << endl;
		// cout << "Stack next type = " << lua_typename(l, lua_type(l, -2)) << endl;

		// The Sandbox worked, and attempted to run the script. Success or failure of the script itself is
		// returned through the stack.
		// Top of Stack = status. 0 = Success, 1 = Script Error (fixible by script author), 2 = Vassal error
		// Stack Top-1  = return value for status == 0, or text error message for status != 0
		// TODO Stack Top-2  = Stack trace or additional error information

		int status = lua_tointeger(l, -1);
		// cout << "LuaEnvironment::execute: status returned " << status << endl;
		lua_pop(l, 1);

		if (status == ScriptResult::eResult_Success) {
			// Script ran successfully
			// TODO Implying that Script & Expressions can only return String value???
			result.setResultValue(make_unique<TValue>(lua_tostring(l, -1)));
		} else {
			// Script failed to run correctly
			handleScriptError (l, result);
		}

	}

	// Clear the Lua stack of anything remaining
	lua_pop(l, lua_gettop(l));

	// Pop the latest context frame off the stack, replacing the current frame and releasing all it's associated Proxy objects.
	popCurrentContext();

	return;
}

ProxyFactory LuaEnvironment::getProxyFactory() const {
	return proxyFactory;
}

int LuaEnvironment::getCurrentContextLevel() const {
	return contextStack.size();
}

/*
 * Create a new Context Frame for this invocation and push it on to the top of the context stack
 */
void LuaEnvironment::pushCurrentContext(const Scriptable *context) {
	contextStack.push_back(make_unique<ContextFrame>(context));
	//currentContextFrame = make_unique<ContextFrame> (context);
	// cout << "LuaEnvironment::pushCurrentContext, size now " << contextStack.size() << endl;

}

/*
 * Pop the current Context Frame off the stack
 */
void LuaEnvironment::popCurrentContext() {
	contextStack.pop_back();

	// Clear the sandbox CPU step counter if no stack frames left. Top level script has completed.
	if (contextStack.size() == 0) {
		resetLuaStepCount();
	}
	// cout << "LuaEnvironment::popCurrentContext, size now " << contextStack.size() << endl;
}

/*
 * Return the current (latest) context frame
 */
unique_ptr<ContextFrame> & LuaEnvironment::getCurrentContext() const {
	return contextStack.back();
}


long LuaEnvironment::incrLuaStepCount() {
	return ++luaStepCount;
}

void LuaEnvironment::resetLuaStepCount() {
	luaStepCount = 0;
}
