/*
 * LuaEnvironment.cpp
 *
 * A LuaEnvironment is a sanboxes Lua interpreter
 *
 *
 *  Created on: 29/07/2017
 *      Author: Brent
 *
 */


#include <Lua/lua.hpp>
#include <Scripting/ContextFrame.h>
#include <Scripting/Lua/LuaEnvironment.h>
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
 * A call to failed call to cbAssert indicates an error in the Vassal to Lua protocol, NOT in the user script code.
 *
 * Needs to be static to be callable from callback
 */
static void cbAssert(lua_State *l, const bool test, const string source, const string message) {
	luaEnvironment ->luaAssert(l, test, source, message);
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
	cbAssert(l, nargs > 0, id, "No arguments supplied in Callback");
	cbAssert(l, lua_isinteger(l, 1), id, "Callback arg 1 is not an integer");
	Scriptable::eType contextType = (Scriptable::eType) lua_tointeger(l, 1);
	id += ":" + Scriptable::getTypeName(contextType);
	cbAssert(l, Scriptable::isValidType(contextType), id, "Callback arg 1 type is not valid (" + to_string(contextType) + ")");
	// cout << "Callback: Target type =" << contextType << endl;

	// 2nd argument must be a pointer to the Scriptable to use as a context
	cbAssert(l, nargs > 1, id, "Callback arg 2 missing");
	cbAssert(l, lua_islightuserdata(l, 2), id, "Callback arg 2 is not a pointer");
	const void *contextPtr = lua_topointer(l, 2);
	// cout << "callback: Target object passed to callback=" << contextPtr << endl;

	// 3rd Argument must be a raw pointer to the current ContextFrame
	cbAssert(l, nargs > 2, id, "Callback arg 3 missing");
	cbAssert(l, lua_islightuserdata(l, 3), id, "Callback arg 3 is not a pointer");
	ContextFrame *myContext = (ContextFrame *) lua_topointer(l, 3);
	// cout << "callback: Context passed to callback=" << myContext << endl;

	// 4th Argument must be the name of the operation to perform on the Scriptable
	cbAssert(l, nargs > 3, id, "Callback arg 4 missing");
	cbAssert(l, lua_isstring(l, 4), id, "Callback arg 4 is not a string");
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
			long long int i = lua_tointeger(l, actualParamIdx++);
			id += ":" + to_string(i);
			// cout << "Callback: op arg " << ++opParamIdx << " Integer=" << i << endl;
			args.push_back(make_unique<TValue>(i));

		} else if (lua_isnumber(l, actualParamIdx)) {
			double d = lua_tonumber(l, actualParamIdx++);
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
			cbAssert(l, actualParamIdx < nargs, id + paramId + "]", "Object arg not followed by parameter type arg");

			// The Object Pointer MUST be immediately followed by an integer representing the parameter type .
//			cbAssert(l, lua_isinteger(l, actualParamIdx), id + paramId + "]", "Object arg not followed by integer arg for parameter type");
//			TValue::eType paramType = (TValue::eType) lua_tointeger(l, actualParamIdx++);
//			cbAssert(l, (paramType == TValue::eType_Vobject || paramType == TValue::eType_Viterator), id + paramId + "]", "Object arg not followed by parameter type of Object or Iterator");
//
//			string ptype = (paramType == TValue::eType_Vobject) ? "Object" : "Iterator";
//			paramId += "." + ptype;

			// The Parameter type MUST be followed by the Object type
			cbAssert(l, actualParamIdx < nargs, id + paramId + "]", " Object arg not followed by object type argument");
			cbAssert(l, lua_isinteger(l, actualParamIdx), id + paramId + "]", " Object arg not followed by integer arg for Object type");
			Scriptable::eType objectType = (Scriptable::eType) lua_tointeger(l, actualParamIdx++);

			cbAssert(l, Scriptable::isValidType(objectType), id +paramId + "]", "Object type is not valid (" + to_string(objectType) + ")");

			// TODO Work out why the hell I get a const correctness error message when calling
			// 		TValue(pointerArg, objectType, paramType)
			// but not when calling the 1 argument version
			// 		TValue(pointerArg)
			// Lua returns the pointer as const void *, can't change that
			unique_ptr<TValue> tv = make_unique<TValue> (pointerArg);
			tv->setObjectType(objectType);


			// Create the TValue and add it to the Argument list
			// args.push_back(new TValue(pointerArg, objectType, paramType));
			args.push_back(move(tv));

			// cout << "Callback: op arg " << ++opParamIdx << " " << ptype << " type"  << endl;

			// Add the parameter id to the callback description string.
			id += paramId;

		} else {
			cbAssert(l, 0, id + "]", "Unknown Argument type from Lua");
		}
	}

	id += "]";

	// cout << "Callback:  - " << id << endl;

	// Ask the Proxy to perform the operation. It will take care of 'talking' to the real Vassal Game Object
	// performOperation returns false if there was a Vassal-caused error in the Lua <> Vassal protocol and returns
	// the error message in error.
	// If performOperation returns true, then there was no Vassal-caused error, but there may still be some script
	// related error that will be returned to the calling script in the result.
	//unique_ptr<TValue> result;
	string error;

	// cout << "Callback: " << id << " Using context at " << myContext << endl;
	// cout << "Callback: " << id << " contextPre= " << contextPtr << endl;

	// if (contextType == Scriptable::eType_Piece) {
		// Piece *p = (Piece *) contextPtr;
		// cout << "Callback: piece name =" << p->getName() << endl;
	// }

	// ProxyPiece *pp = (ProxyPiece *) myContext->getProxy(contextType, contextPtr).get();

	// cout << "Callback: Context returned ProxyPiece at "<< pp << endl;
	// cout << "Callback: ProxyPiece contains Piece is at " << pp ->getPiece() << endl;
	// cout << "Callback: Arg count is " << args.size() <<  endl;

	ScriptResult scriptResult;

	// bool success = myContext->getProxy(contextType, contextPtr) -> performOperation(operation, args, result, error);
	myContext->getProxy(contextType, contextPtr) -> performOperation(operation, args, scriptResult);


	// Pop the args that where sent to us from Lua off the stack. These cannot be popped prior to this as their value
	// becomes undefined.
	lua_pop(l, nargs);

	// If there is an error trying to talk to the Vassal object, abort!
	cbAssert (l, scriptResult.getResultLevel() == ScriptResult::eResult_Success, id, scriptResult.getError());

	int returnArgsCount = 0;
	// unique_ptr<TValue> result = move(scriptResult.getResult());
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
		cbAssert(l, 0, id, "TValue from Vassal with unknown type (" + to_string(result.getType()) + ")"); /* Unknown TValue type */
		break;
	}

	return returnArgsCount;
}

// TODO Move these into LuaEnvironment, and access through *luaEnvironment


static void hook(lua_State* L, lua_Debug *ar)
{
	if (luaEnvironment->incrLuaStepCount() > LuaEnvironment::LUA_STEP_LIMIT) {
		luaL_error (L, "Script using too much CPU");
	}
	if (lua_gc (L, LUA_GCCOUNT, 0) > LuaEnvironment::LUA_MEMORY_LIMIT) {
		luaL_error (L, "Script using too much Memory");
	}
}

LuaEnvironment::LuaEnvironment() {
	isInitialised = false;
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
			initialisationError = "Lua Environment failed to initialised. Error compiling "+INIT_FILE_NAME+": "+error;
			return;
		}

		if (lua_pcall(l, 0, LUA_MULTRET, 0)) {
			string error = lua_tostring(l, -1);
			lua_pop(l, 1);
			initialisationError = "Lua Environment failed to initialised. Error running "+INIT_FILE_NAME+": "+error;
			return;
		}

		// Register the callback function
		lua_register(l, "callback", callback);

		// Debug hook
		lua_sethook(l, hook, LUA_MASKCOUNT, LuaEnvironment::LUA_STEP_DELTA);

		isInitialised = true;
	}
}

void LuaEnvironment::setState(lua_State* state) {
	this->state = state;
}

lua_State* LuaEnvironment::getState() {
	return this->state;
}

const bool LuaEnvironment::initialised() const {
	return isInitialised;
}

/*!
 * \brief Has the Scripting Environment been successfully initialised?
 */
const string LuaEnvironment::getInitialisationError() const {
	return initialisationError;
}

void LuaEnvironment::validate(Script *expression) {

	lua_State *l = getState();

	// Get Lua to compile the expression (this does not run it)
	int i = luaL_loadstring(l, expression->getExecutableSource().c_str());

	// Invalid, set the error into the expression, pop the error off the Lua stack and return
	if (i) {
		expression->setError(lua_tostring(l, -1));
		lua_pop(l, 1);
		return;
	}

	// No error, clear the error flag in the expression and pop the compiled expression off the Lua stack.
	lua_pop(getState(), 1);
	expression->setError("");
	return;
}

void LuaEnvironment::execute(Script *script, Scriptable *thisPtr) {

	lua_State *l = getState();

	// Create a new ContextFrame containing the 'this' proxy on to the top of the Context Stack
	pushCurrentContext(thisPtr);

	// Load the sandbox code executor _M.eval
	lua_getglobal(l, "_M");
	lua_getfield(l, -1, "eval");

	// And push the required arguments
	// The Parameter sequence for calling _M.eval is
	// 1. The source of the script to be executed
	// 2. The name of the script
	// 3. The Scriptable::eType of the Vassal Object that the script is running against
	// 4. A (void *) ptr to the to the Proxy object that wraps the Vassal Object.
	// 5. A (void *) ptr to the current ContextFrame containing all Proxy Objects used in this script invocation
	//
	lua_pushstring(l, script->getExecutableSource().c_str());			 // Script code
	lua_pushstring(l, script->getName().c_str());						 // Script name
	lua_pushinteger(l, thisPtr->getScriptableType());	 				 // Context type
	lua_pushlightuserdata(l, ((void *) thisPtr));    				     // Context reference
	lua_pushlightuserdata(l, ((void *) getCurrentContext().get()));      // Context Framereference

	// Call the compiled code.
	if (lua_pcall(l, 5, LUA_MULTRET, 0)) {
		// Something went wrong with the sandbox execution
		string error = "Sandbox Failure";
		error.append(lua_tostring(l, -1));
		script->setError(error);
		script->setResult("");
		lua_pop(l, 1);
	} else {
		// Sandbox worked
		if (lua_isnil(l, -2)) {
			// Expression failed to evaluate
			script->setError(lua_tostring(l, -1));
			script->setResult("");
			lua_pop(l, 2);
		} else {
			// Expression evaluated
			script->setResult(lua_tostring(l, -1));
			script->setError("");
			lua_pop(l, 1);
		}
	}

	// Pop the last context frame back off the stack, replacing the current frame and releasing all it's associated Proxy objects.
	popCurrentContext();

	return;
}

ProxyFactory LuaEnvironment::getProxyFactory() const {
	return proxyFactory;
}

// Push a new context frame on to the top of the stack
void LuaEnvironment::pushCurrentContext(Scriptable *context) {
	contextStack.push_back(make_unique<ContextFrame>(context));
	//currentContextFrame = make_unique<ContextFrame> (context);
	// cout << "LuaEnvironment::pushCurrentContext Created new context Frame at " << currentContextFrame.get() << endl;

}

// Pop the top context off the top.
void LuaEnvironment::popCurrentContext() {
	contextStack.pop_back();

	// Clear the sandbox CPU step counter if no stack frames left. Top level script has completed.
	if (contextStack.size() == 0) {
		resetLuaStepCount();
	}
}

unique_ptr<ContextFrame> & LuaEnvironment::getCurrentContext() const {
	return contextStack.back();
}


// Handle fate errors within the LuaEnvironment
void LuaEnvironment::luaAssert(lua_State *l, const bool test, const string source, const string message) {
	if (!test) {
		lua_pushstring(l, ("Vassal error (" + source + "): " + message).c_str());
		lua_error(l);
	}
}

long LuaEnvironment::incrLuaStepCount() {
	return ++luaStepCount;
}

void LuaEnvironment::resetLuaStepCount() {
	luaStepCount = 0;
}
