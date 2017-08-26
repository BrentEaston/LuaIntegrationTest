/*
 * LuaError_test.cpp
 *
 *  Created on: 24 Aug. 2017
 *      Author: Brent
 */

#include <Lua/lua.hpp>
#include <gtest/gtest.h>
#include <Scripting/Lua/LuaEnvironment.h>
#include <Scripting/Lua/LuaError.h>
#include <memory>

using namespace std;

// The fixture for testing class Foo.
class LuaError_test: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.


	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
		env = make_unique<LuaEnvironment>();
		l = env->getState();
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Foo.
	unique_ptr<LuaEnvironment> env;
	lua_State *l;
};

// Basic error message, no source
TEST_F(LuaError_test, BasicErrorString) {
	string message = "Error Message";
	int stackLevel = 2;
	ScriptResult::eResult errorLevel = ScriptResult::eResult_Script_Error;

	lua_pushstring(l, message.c_str());
	lua_pushinteger(l, errorLevel);
	lua_pushinteger(l, stackLevel);
	LuaError e1 (l);
	e1.build();

	EXPECT_EQ (stackLevel, e1.getStackLevel());
	EXPECT_EQ (errorLevel, e1.getErrorLevel());
	EXPECT_EQ (message, e1.getErrorMessage());
	EXPECT_EQ (0, e1.getSourceLine());
	EXPECT_EQ ("", e1.getSourceName());

}

// Basic Error message plus source passed in 4th argument
TEST_F(LuaError_test, BasicErrorStringPlusName) {
	string message = "Error Message";
	int stackLevel = 2;
	ScriptResult::eResult errorLevel = ScriptResult::eResult_Script_Error;
	string sourceName = "TestRoutine";

	lua_pushstring(l, message.c_str());
	lua_pushinteger(l, errorLevel);
	lua_pushinteger(l, stackLevel);
	lua_pushstring(l, sourceName.c_str());
	LuaError e1 (l);
	e1.build();

	EXPECT_EQ (stackLevel, e1.getStackLevel());
	EXPECT_EQ (errorLevel, e1.getErrorLevel());
	EXPECT_EQ (message, e1.getErrorMessage());
	EXPECT_EQ (0, e1.getSourceLine());
	EXPECT_EQ (sourceName, e1.getSourceName());

}

// Error message referencing init.lua
TEST_F(LuaError_test, InitErrorString) {
	string message = "init.lua:283: Error Message";
	int stackLevel = 2;
	ScriptResult::eResult errorLevel = ScriptResult::eResult_Script_Error;

	lua_pushstring(l, message.c_str());
	lua_pushinteger(l, errorLevel);
	lua_pushinteger(l, stackLevel);
	LuaError e1 (l);
	e1.build();

	EXPECT_EQ (stackLevel, e1.getStackLevel());
	EXPECT_EQ (errorLevel, e1.getErrorLevel());
	EXPECT_EQ ("Error Message", e1.getErrorMessage());
	EXPECT_EQ (0, e1.getSourceLine());
	EXPECT_EQ ("", e1.getSourceName());

}

// Error message referencing init.lua plus source name passed as 4th parameter
TEST_F(LuaError_test, InitErrorStringPlusName) {
	string message = "init.lua:283: Error Message";
	int stackLevel = 2;
	ScriptResult::eResult errorLevel = ScriptResult::eResult_Script_Error;
	string sourceName = "TestRoutine";

	lua_pushstring(l, message.c_str());
	lua_pushinteger(l, errorLevel);
	lua_pushinteger(l, stackLevel);
	lua_pushstring(l, sourceName.c_str());
	LuaError e1 (l);
	e1.build();

	EXPECT_EQ (stackLevel, e1.getStackLevel());
	EXPECT_EQ (errorLevel, e1.getErrorLevel());
	EXPECT_EQ ("Error Message", e1.getErrorMessage());
	EXPECT_EQ (0, e1.getSourceLine());
	EXPECT_EQ (sourceName, e1.getSourceName());

}

// Error message referencing a user script
TEST_F(LuaError_test, ScriptErrorString) {
	string message = "[string \"TestRoutine\"]:283: Error Message";
	int stackLevel = 2;
	ScriptResult::eResult errorLevel = ScriptResult::eResult_Script_Error;
	string sourceName = "TestRoutine";

	lua_pushstring(l, message.c_str());
	lua_pushinteger(l, errorLevel);
	lua_pushinteger(l, stackLevel);
	LuaError e1 (l);
	e1.build();

	EXPECT_EQ (stackLevel, e1.getStackLevel());
	EXPECT_EQ (errorLevel, e1.getErrorLevel());
	EXPECT_EQ ("Error Message", e1.getErrorMessage());
	EXPECT_EQ (283, e1.getSourceLine());
	EXPECT_EQ ("TestRoutine", e1.getSourceName());

}

// Test Error object
TEST_F(LuaError_test, ErrorObject) {
	string message = "Error Message";
	int stackLevel = 2;
	ScriptResult::eResult errorLevel = ScriptResult::eResult_Script_Error;
	string sourceName = "TestRoutine";
	int sourceLine = 283;
	string fullError = "[TestRoutine]:283: Error Message";

	lua_newtable(l);

	lua_pushinteger(l, errorLevel);
	lua_setfield(l, -2, LuaError::LEVEL_TAG);

	lua_pushstring(l,message.c_str());
	lua_setfield(l, -2, LuaError::ERROR_TAG);

	lua_pushstring(l, sourceName.c_str());
	lua_setfield(l, -2, LuaError::SCRIPT_TAG);

	lua_pushinteger(l, sourceLine);
	lua_setfield(l, -2, LuaError::LINE_TAG);

	LuaError e1 (l);
	e1.build();

	EXPECT_EQ (stackLevel, e1.getStackLevel());
	EXPECT_EQ (errorLevel, e1.getErrorLevel());
	EXPECT_EQ (message, e1.getErrorMessage());
	EXPECT_EQ (sourceLine, e1.getSourceLine());
	EXPECT_EQ (sourceName, e1.getSourceName());
	EXPECT_EQ (fullError, e1.formatError());

	// Clear the stack
	lua_pop(l, lua_gettop(l));

	// Push an error object on to the stack
	e1.pushError();

	// Read it back and test it is the same
	LuaError e2 (l);
	e2.build();

	EXPECT_EQ (stackLevel, e2.getStackLevel());
	EXPECT_EQ (errorLevel, e2.getErrorLevel());
	EXPECT_EQ (message, e2.getErrorMessage());
	EXPECT_EQ (sourceLine, e2.getSourceLine());
	EXPECT_EQ (sourceName, e2.getSourceName());
	EXPECT_EQ (fullError, e2.formatError());

}
