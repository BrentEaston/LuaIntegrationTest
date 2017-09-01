/*
 * Script.cpp
 *
 *  Created on: 31/07/2017
 *      Author: Brent
 */

/***
 * A Script that will be run by the Scripting engine.
 *
 */

#include <Scripting/Script.h>
#include <Scripting/ScriptResult.h>
#include <Scripting/ScriptingEnvironment.h>
#include <Scripting/ScriptingFactory.h>
#include <memory>

using namespace std;

/** Constructors */
Script::Script(ScriptingEnvironment *env, const std::string script, const eReturnType type) {
	set (env, script, "", type);
}

Script::Script(ScriptingEnvironment *env, const std::string script) {
	set (env, script, "", eReturnType_String);
}

Script::Script(ScriptingEnvironment *env, const std::string script, const std::string name) {
	set (env, script, name, eReturnType_String);

}

Script::Script(ScriptingEnvironment *env, const std::string script, const std::string name, const eReturnType type) {
	set (env, script, name, type);
}

/** Destructor */
Script::~Script() {
	// TODO Auto-generated destructor stub
}

void Script::set (ScriptingEnvironment *env, const std::string script, const std::string name, const eReturnType returnType) {
	this -> source = script;
	this -> name = name;
	this -> returnType = returnType;
	this -> environment = env;
}


/** Getter/Setter for source */
const std::string Script::getSource() const {
	return source;
}

std::string Script::getExecutableSource() const {
	return source;
}

const Script::eReturnType Script::getReturnType() const {
	return returnType;
}

const std::string Script::getName() const {
	return name;
}

/**
 * Evaluate the script using the supplied context.  VassalContext provides a getProperty(name) call
 * that will be used by the evaluation environment to evaluate Vassal properties referenced in the script
 */
void Script::execute (const Scriptable *context, ScriptResult &result) {

	// Execute the script
	// ScriptingFactory::getInstance() -> getEnvironment() -> execute(this, context);
	environment->execute(this, context, result);
	return;

//	if (! hasError()) {
//		return true;
//	}
//
//	return false;
}

/** Validate the script for syntactic correctness. Do not attempt to evaluate it, or save any reference to it.
 * */
void Script::validate (ScriptResult &result) {

	// Validate the script
	// ScriptingFactory::getInstance() -> getEnvironment() -> validate (this);
	environment -> validate (this, result);
	return;
//	if (hasError()) {
//		return false;
//	}
//
//	// Just return true if it succeeds validation
//	return true;

}
