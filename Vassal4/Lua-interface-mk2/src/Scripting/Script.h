/*
 * Script.h
 *
 *  Created on: 31/07/2017
 *      Author: Brent
 */

#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <string>

class ScriptingEnvironment;
class ScriptResult;

class Scriptable;

class Script {
public:

	enum eReturnType {eReturnType_Invalid, eReturnType_Int, eReturnType_Float, eReturnType_String, eReturnType_None };

	Script(ScriptingEnvironment *env, const std::string script);
	Script(ScriptingEnvironment *env, const std::string script, const std::string name);
	Script(ScriptingEnvironment *env, const std::string script, const std::string name, const eReturnType type);
	Script(ScriptingEnvironment *env, const std::string script, const eReturnType type);
	virtual ~Script();

	std::string getSource() const;
	virtual std::string getExecutableSource() const;

	eReturnType getReturnType() const;

	void execute (const Scriptable *context, ScriptResult &result);
	void validate (ScriptResult &result);

	std::string getName() const;

protected:

	void set (ScriptingEnvironment *env, const std::string script, const std::string name, const eReturnType returnType);
	void clear();

	std::string name;
	std::string source;
	eReturnType returnType;
//	std::string result;
//	std::string error;

	ScriptingEnvironment *environment;
};

#endif /* SCRIPT_H_ */
