/*
 * Expression_test.cpp
 *
 *  Created on: 08/08/2017
 *      Author: Brent
 */

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>
#include "Scripting/TValue.h"
#include "Piece.h"

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

  void IntTester (const int i) {
	  	TValue t (i);
	    EXPECT_EQ (TValue::eType_integer, t.getType());
	    EXPECT_EQ (i, t.getIntegerValue());
	    EXPECT_TRUE(t.isInteger());
	    EXPECT_FALSE(t.isNil());
	    EXPECT_FALSE(t.isFloat());
	    EXPECT_FALSE(t.isBoolean());
	    EXPECT_FALSE(t.isString());
	    EXPECT_FALSE(t.isObject());
	    EXPECT_EQ (std::to_string(i), t.getValueAsString());
	    EXPECT_EQ ("Integer: "+std::to_string(i), t.toString());
  }

  void FloatTester (const float f) {
	  	TValue t (f);
	    EXPECT_EQ (TValue::eType_float, t.getType());
	    EXPECT_EQ (f, t.getFloatValue());
	    EXPECT_FALSE(t.isInteger());
	    EXPECT_FALSE(t.isNil());
	    EXPECT_TRUE(t.isFloat());
	    EXPECT_FALSE(t.isBoolean());
	    EXPECT_FALSE(t.isString());
	    EXPECT_FALSE(t.isObject());
	    EXPECT_EQ (std::to_string(f), t.getValueAsString());
	    EXPECT_EQ ("Float: "+std::to_string(f), t.toString());
  }

  std::string boolToString (const bool b) {
	  return b ? "true" : "false";
  }

  void BoolTester (const bool b) {
	  	TValue t (b);
	    EXPECT_EQ (TValue::eType_boolean, t.getType());
	    EXPECT_EQ (b, t.getBooleanValue());
	    EXPECT_FALSE(t.isInteger());
	    EXPECT_FALSE(t.isNil());
	    EXPECT_FALSE(t.isFloat());
	    EXPECT_TRUE(t.isBoolean());
	    EXPECT_FALSE(t.isString());
	    EXPECT_FALSE(t.isObject());
	    EXPECT_EQ (boolToString(b), t.getValueAsString());
	    EXPECT_EQ ("Boolean: "+boolToString(b), t.toString());
  }

  void StringTester (const std::string s) {
	  	TValue t (s);
	    EXPECT_EQ (TValue::eType_string, t.getType());
	    EXPECT_EQ (s, t.getStringValue());
	    EXPECT_FALSE(t.isInteger());
	    EXPECT_FALSE(t.isNil());
	    EXPECT_FALSE(t.isFloat());
	    EXPECT_FALSE(t.isBoolean());
	    EXPECT_TRUE(t.isString());
	    EXPECT_FALSE(t.isObject());
	    EXPECT_EQ (s, t.getValueAsString());
	    EXPECT_EQ ("String: "+s, t.toString());
  }

};

TEST_F(TValue_Test, IntConstructor) {
	IntTester (1234);
	IntTester (0);
	IntTester (-5);
};

TEST_F (TValue_Test, NilConstructor) {
	TValue *t = new TValue ();
	EXPECT_EQ (TValue::eType_nil, t->getType());
	EXPECT_FALSE(t->isInteger());
    EXPECT_TRUE(t->isNil());
    EXPECT_FALSE(t->isFloat());
	EXPECT_FALSE(t->isBoolean());
	EXPECT_FALSE(t->isString());
	EXPECT_FALSE(t->isObject());
	EXPECT_EQ ("", t->getValueAsString());
	EXPECT_EQ ("Nil: ", t->toString());

	delete t;
}

TEST_F(TValue_Test, FloatConstructor) {
	FloatTester (1234.2);
	FloatTester (0.0);
	FloatTester (-5.234);
};

TEST_F(TValue_Test, BooleanConstructor) {
	BoolTester (true);
	BoolTester (false);
};

TEST_F(TValue_Test, StringConstructor) {
	StringTester ("test1");
	StringTester ("");
};

TEST_F(TValue_Test, ObjectConstructor) {
	Piece *p = new Piece ("xyzzy", 4);
	TValue t ((void *) p, p->getScriptableType());
	EXPECT_EQ (TValue::eType_Vobject, t.getType());
	EXPECT_FALSE(t.isInteger());
    EXPECT_FALSE(t.isNil());
    EXPECT_FALSE(t.isFloat());
	EXPECT_FALSE(t.isBoolean());
	EXPECT_FALSE(t.isString());
	EXPECT_TRUE(t.isObject());
	EXPECT_EQ ("Piece", t.getValueAsString());
	EXPECT_EQ ("Object: Piece", t.toString());
	EXPECT_EQ (p->getScriptableType(), t.getObjectType());
	EXPECT_EQ ((void *) p, t.getObjectPointer());


}
} // Namespace

