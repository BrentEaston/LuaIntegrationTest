/*
 * ProxyIterator_test.cpp
 *
 *  Created on: 17 Aug. 2017
 *      Author: Brent
 */

#include "gtest/gtest.h"
#include <Scripting/ProxyIterator.h>

// The fixture for testing class Foo.
class ProxyIterator_test: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	ProxyIterator_test() {
		// You can do set-up work for each test here.
	}

	virtual ~ProxyIterator_test() {
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

TEST(ProxyIterator_test, test1) {
	int a = 1;
	int b = 2;
	EXPECT_EQ(1, b - 1);
}

