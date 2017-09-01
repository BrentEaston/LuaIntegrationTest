/*
 * ScriptError_test.cpp
 *
 *  Created on: 31 Aug. 2017
 *      Author: Brent
 */

#include "gtest/gtest.h"
#include <Scripting/ScriptError.h>

// The fixture for testing class Foo.
class ScriptError_test: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	ScriptError_test() {
		// You can do set-up work for each test here.
	}

	virtual ~ScriptError_test() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(ScriptError_test, ConstructorTest) {
	string message = "Error message";
	ScriptResult::eResult resultLevel = ScriptResult::eResult_Script_Error;
	string name = "TestScript";
	string info = "Error message\nstack traceback:\nStack Level 1\nStack Level 2";

	ScriptError error (message, resultLevel, name, info);

	EXPECT_EQ (message, error.getMessage());
	EXPECT_EQ (resultLevel, error.getLevel());
	EXPECT_EQ (name, error.getSource());
	EXPECT_EQ (info, error.getInfo());
}

