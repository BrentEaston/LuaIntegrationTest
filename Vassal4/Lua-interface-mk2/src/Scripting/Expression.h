/*
 * Expression.h
 *
 *  Created on: 08/08/2017
 *      Author: Brent
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <Scripting/Script.h>
#include <Scripting/ScriptingEnvironment.h>
#include <string>

/***
 * An Expression is a simple script that returns a value. It is designed to implement Vassal 3 'Calculated Properties' and in-line expressions.
 */
class Expression: public Script {
public:
	Expression(ScriptingEnvironment *env, const std::string script, const std::string name) :
			Script(env, script, name) {
	};

	Expression(ScriptingEnvironment *env, const std::string script, const std::string name, const eReturnType type) :
			Script(env, script, name, type) {
	};

	Expression(ScriptingEnvironment *env, const std::string script, const eReturnType type) :
			Script(env, script, type) {
	};

	virtual ~Expression();

	std::string getExecutableSource() const;
};

#endif /* EXPRESSION_H_ */
