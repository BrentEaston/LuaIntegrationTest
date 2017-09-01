/*
 * Script_test.cpp
 *
 *  Created on: 31/07/2017
 *      Author: Brent
 */

#include <gtest/gtest.h>
#include <Map.h>
#include <Piece.h>
#include <Scripting/ScriptingEnvironment.h>
#include <Scripting/ScriptingFactory.h>
#include <Scripting/ScriptResult.h>
#include "Scripting/Script.h"
#include <memory>

using namespace std;

class LuaEnvironment;

// The fixture for testing class Script.
class Script_test: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	Script_test() {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	virtual ~Script_test() {
		// You can do clean-up work that doesn't throw exceptions here.

	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
		piece = new Piece ("Uchi-mata", 4);
		map = new Map ("Lothlorien");
		piece->setMap(map);
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
		delete map;
		delete piece;
	}

	// Objects declared here can be used by all tests in the test case for Foo.

	Piece *piece;
	Map *map;
	ScriptResult result;

	ScriptingEnvironment *environment = ScriptingFactory::getInstance() -> getEnvironment().get();

	bool startsWith (string in, string lookup) {
		return in.length() >= lookup.length() && in.find(lookup) == 0;
	}

	bool contains (string in, string lookup) {
		return in.length() >= lookup.length() && in.find(lookup) < string::npos;
	}

};

TEST_F(Script_test, ConstructorTests) {

	string script = "local x = 10";
	string name = "Test";
	Script::eReturnType returnType = Script::eReturnType_None;

	Script s1 (environment, script, name, returnType);
	EXPECT_EQ (script, s1.getSource());
	EXPECT_EQ (script, s1.getExecutableSource());
	EXPECT_EQ (returnType, s1.getReturnType());
	EXPECT_EQ (name, s1.getName());

	Script s2 (environment, script, name);
	EXPECT_EQ (script, s2.getSource());
	EXPECT_EQ (script, s2.getExecutableSource());
	EXPECT_EQ (Script::eReturnType_String, s2.getReturnType());
	EXPECT_EQ (name, s2.getName());

	Script s3 (environment, script, returnType);
	EXPECT_EQ (script, s3.getSource());
	EXPECT_EQ (script, s3.getExecutableSource());
	EXPECT_EQ (Script::eReturnType_None, s3.getReturnType());
	EXPECT_EQ ("", s3.getName());

	Script s4 (environment, script);
	EXPECT_EQ (script, s4.getSource());
	EXPECT_EQ (script, s4.getExecutableSource());
	EXPECT_EQ (Script::eReturnType_String, s4.getReturnType());
	EXPECT_EQ ("", s4.getName());

}

TEST_F(Script_test, ValidateTests) {

	string goodScript = "local x = 10";
	string badScript = "local x = 10\nlocal y = x + + 20";
	string name = "Test";

	Script s1 (environment, goodScript, name);
	s1.validate(result);
	EXPECT_TRUE (result.isSuccessful());

	Script s2 (environment, badScript, name);
	s2.validate(result);
	EXPECT_FALSE (result.isSuccessful());
}

TEST_F(Script_test, SandboxTests) {

	Script s1 (environment, "local x = 'Time='..os.time()..', Test succeeded!'", "Test1");
	s1.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Check Global Variables redirected from proxy table to real table. Error: " << result.getError();

}
TEST_F(Script_test, RawgetSandbox) {
	// Test rawget sandboxing
	Script s2 (environment, "local tab = {22, 33}\nlocal x = rawget(tab, 1)", "Test2");
	s2.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Check rawget available on local table. Error: " << result.getError();

	Script s3 (environment, "local x = _ENV local y = rawget(x, 'Hi')", "Test3");
	s3.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check rawget blocked on global table.";
	EXPECT_TRUE (startsWith(result.getError(), "May not use rawget on a locked table")) << "Unexpected error message: " << result.getError();
}
TEST_F(Script_test, RawsetSandbox) {
	// Test rawset sandboxing
	Script s4 (environment, "local tab = {22, 33}\nrawset(tab, 3, 'Test')", "Test4");
	s4.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Check rawset available on local table. Error: " << result.getError();

	Script s5 (environment, "local x = _ENV rawset(x, 'Hi', 'there')", "Test5");
	s5.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check rawset blocked on global table.";
	EXPECT_TRUE (startsWith(result.getError(), "May not use rawset on a locked table")) << "Unexpected error message: " << result.getError();
}
TEST_F(Script_test, SetmetatableSandbox) {
	// Test setmetatable sandboxing
	Script s6 (environment, "local tab = {22, 33}\nsetmetatable(tab, {})", "Test6");
	s6.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Check setmetatable available on local table. Error: " << result.getError();

	Script s7 (environment, "local x = _ENV setmetatable(x, {})", "Test7");
	s7.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check setmetatable blocked on global table.";
	EXPECT_TRUE (startsWith(result.getError(), "May not use setmetatable on a locked table")) << "Unexpected error message: " << result.getError();
}
TEST_F(Script_test, GetmetatableSandbox) {
	// Test getmetatable sandboxing
	Script s8 (environment, "local tab = {22, 33}\nlocal y = getmetatable(tab)", "Test8");
	s8.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Check getmetatable available on local table. Error: " << result.getError();

	Script s9 (environment, "local x = _ENV local y = getmetatable(x)", "Test9");
	s9.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check getmetatable blocked on global table.";
	EXPECT_TRUE (startsWith(result.getError(), "May not use getmetatable on a locked table")) << "Unexpected error message: " << result.getError();
}
TEST_F(Script_test, GlobalVariableSandbox) {
	// Attempt to set a global variable
	Script s10 (environment, "xyzzy=42");
	s10.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check unable to create global variables";
	EXPECT_TRUE (startsWith(result.getError(), "Attempting to set Global Variable")) << "Unexpected error message: " << result.getError();

	// Attempt to read a global variable
	Script s11 (environment, "local x = xyzzy");
	s11.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check error attempting to read global variables";
	EXPECT_TRUE (startsWith(result.getError(), "Attempting to read undefined value")) << "Unexpected error message: " << result.getError();
}
TEST_F(Script_test, StringRepSandbox) {
	// Check string.rep() limits number of replications
	Script s12 (environment, "local x = string.rep('x', 100)");
	s12.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check replication limit for string.rep()";
	EXPECT_TRUE (contains (result.getError(), "string.rep() too many replications")) << "Unexpected error message: " << result.getError();

	Script s13 (environment, "local x = ('42'):rep(100)");
	s13.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check replication limit for ('42'):rep()";
	EXPECT_TRUE (contains (result.getError(), "string.rep() too many replications")) << "Unexpected error message: " << result.getError();

	// Check string.rep() works for small number of replications
	Script s14 (environment, "local x = string.rep('x', 10)");
	s14.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Replication limit for string.rep() too low";

	Script s15 (environment, "local x = ('x'):rep(10)");
	s15.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Replication limit for ('x'):rep(10) too low, error: " << result.getError();
}
TEST_F(Script_test, StringFindSandbox) {
	Script s15 (environment, "local s = '01234567890123456789012345678901234567890123456789' s:find('.*.*.*.*.*.*.*.*.*.*.*x')");
	s15.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Complex pattern match failure";
	EXPECT_TRUE (contains (result.getError(), "match Pattern too complex")) << "Unexpected error message: " << result.getError();
}
TEST_F(Script_test, CpuLimitSandbox) {
	// Check CPU limitation
	Script s16 (environment, "local function fib(n) return n<2 and n or fib(n-1)+fib(n-2) end fib(37)");
	s16.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "CPU Limitation check failed";
	EXPECT_TRUE (startsWith (result.getError(), "Script using too much CPU")) << "Unexpected error message: " << result.getError();
}
TEST_F(Script_test, MemoryLimitSandbox) {
	// Check Memory limitation
	Script s17 (environment, "local x = 'test' for i = 1, 100 do x = x .. x end");
	s17.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Memory Limitation Check failed";
	EXPECT_TRUE (startsWith (result.getError(), "Script using too much Memory")) << "Unexpected error message: " << result.getError();

}
