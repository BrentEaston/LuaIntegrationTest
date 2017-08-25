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

	Script s2 (environment, "local tab = {22, 33}\nlocal x = rawget(tab, 1)", "Test2");
	s2.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Check rawget available on local table. Error: " << result.getError();

	Script s3 (environment, "local x = _ENV local y = rawget(x, 'Hi')", "Test3");
	s3.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check rawset blocked on global table. Error: " << result.getError();

	Script s4 (environment, "local tab = {22, 33}\nrawset(tab, 3, 'Test')", "Test4");
	s4.execute(piece, result);
	EXPECT_TRUE (result.isSuccessful()) << "Check rawset available on local table. Error: " << result.getError();

	Script s5 (environment, "local x = _ENV rawset(x, 'Hi', 'there')", "Test5");
	s5.execute(piece, result);
	EXPECT_FALSE (result.isSuccessful()) << "Check rawset blocked on global table. Error: " << result.getError();

}
