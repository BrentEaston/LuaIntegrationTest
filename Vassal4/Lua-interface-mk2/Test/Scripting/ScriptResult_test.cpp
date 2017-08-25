/*
 * ScriptResult_test.cpp
 *
 *  Created on: 17 Aug. 2017
 *      Author: Brent
 */

#include <gtest/gtest.h>
#include <Scripting/ScriptResult.h>
#include <Scripting/TValue.h>
#include <memory>
#include <string>

// The fixture for testing class Foo.
class ScriptResult_test: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	ScriptResult_test() {
		// You can do set-up work for each test here.
	}

	virtual ~ScriptResult_test() {
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

TEST(ScriptResult_test, BasicTests) {
	ScriptResult result;
	EXPECT_EQ (result.getResultLevel(), ScriptResult::eResult_Success);
	EXPECT_TRUE (result.isSuccessful());
	EXPECT_FALSE (result.isScriptError());
	EXPECT_FALSE (result.isVassalError());
	EXPECT_EQ (result.getError(), "");
	EXPECT_TRUE (result.getResult()->isNil());


	std::string scriptError = "Error in script";
	result.setScriptError(scriptError);
	EXPECT_EQ (result.getResultLevel(), ScriptResult::eResult_Script_Error);
	EXPECT_FALSE (result.isSuccessful());
	EXPECT_TRUE (result.isScriptError());
	EXPECT_FALSE (result.isVassalError());
	EXPECT_EQ (result.getError(), scriptError);

	result.clearError();
	EXPECT_TRUE (result.isSuccessful());
	EXPECT_FALSE (result.isScriptError());
	EXPECT_FALSE (result.isVassalError());

	std::string vassalError = "Error in Vassal";
	result.setVassalError(vassalError);
	EXPECT_EQ (result.getResultLevel(), ScriptResult::eResult_Vassal_Error);
	EXPECT_FALSE (result.isSuccessful());
	EXPECT_FALSE (result.isScriptError());
	EXPECT_TRUE (result.isVassalError());
	EXPECT_EQ (result.getError(), vassalError);

	result.setResultValue(std::make_unique<TValue> (123));
	EXPECT_EQ (result.getResultLevel(), ScriptResult::eResult_Success);
	EXPECT_TRUE (result.isSuccessful());
	EXPECT_EQ (result.getError(), "");
	EXPECT_TRUE (result.getResult()->isInteger());


}

