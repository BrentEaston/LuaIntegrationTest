/*
 * Expression_test.cpp
 *
 *  Created on: 08/08/2017
 *      Author: Brent
 */

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>
#include "Scripting/TValue.h"

namespace {

// The fixture for testing class Foo.
class TValue_Test : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

	TValue_Test() {

	}

  virtual ~TValue_Test() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).

  }

  virtual void TearDown() {

  }

  // Objects declared here can be used by all tests in the test case for Foo.


};

TEST_F(TValue_Test, 1ArgConstructor) {
	  EXPECT_EQ (1, 2-1);
};

} // Namespace

