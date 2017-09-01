/*
 * LuaScriptError_test.cpp
 *
 *  Created on: 31 Aug. 2017
 *      Author: Brent
 */

#include <Lua/lua.hpp>
#include <gtest/gtest.h>
#include <Scripting/Lua/LuaScriptError.h>
#include <Scripting/Lua/LuaEnvironment.h>
#include <memory>

// The fixture for testing class Foo.
class LuaScriptError_test: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	LuaScriptError_test() {
		// You can do set-up work for each test here.
	}

	virtual ~LuaScriptError_test() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

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


// Test Error object can be pushed on the stack and read back without change
TEST_F(LuaScriptError_test, ErrorObject) {
	string message = "Error Message";
	ScriptResult::eResult errorLevel = ScriptResult::eResult_Script_Error;
	string sourceName = "TestRoutine";
	string info = "Error message\nstack traceback:\nStack Level 1\nStack Level 2";
	//string fullError = "[TestRoutine]:283: Error Message";

	lua_newtable(l);

	lua_pushinteger(l, errorLevel);
	lua_setfield(l, -2, LuaScriptError::LEVEL_TAG);

	lua_pushstring(l,message.c_str());
	lua_setfield(l, -2, LuaScriptError::ERROR_TAG);

	lua_pushstring(l, sourceName.c_str());
	lua_setfield(l, -2, LuaScriptError::SOURCE_TAG);

	lua_pushstring(l, info.c_str());
	lua_setfield(l, -2, LuaScriptError::INFO_TAG);

	LuaScriptError e1 (l);
	e1.build();

	EXPECT_EQ (errorLevel, e1.getLevel());
	EXPECT_EQ (message, e1.getMessage());
	EXPECT_EQ (sourceName, e1.getSource());
	EXPECT_EQ (info, e1.getInfo());

	// Clear the stack
	lua_pop(l, lua_gettop(l));

	// Push an error object on to the stack
	e1.push();

	// Read it back and test it is the same
	LuaScriptError e2 (l);
	e2.build();

	EXPECT_EQ (errorLevel, e2.getLevel());
	EXPECT_EQ (message, e2.getMessage());
	EXPECT_EQ (sourceName, e2.getSource());
	EXPECT_EQ (info, e2.getInfo());

}
