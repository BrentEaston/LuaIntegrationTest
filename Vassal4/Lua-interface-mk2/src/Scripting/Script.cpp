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
#include <Scripting/ScriptingEnvironment.h>
#include <Scripting/ScriptingFactory.h>
#include <memory>

using namespace std;

/** Constructors */
Script::Script(const std::string script, const eReturnType type) {
	set (script, "", type);
	clear();
}

Script::Script(const std::string script) {
	set (script, "", eReturnType_String);
	clear();
}

Script::Script(const std::string script, const std::string name) {
	set (script, name, eReturnType_String);
	clear();

}

Script::Script(const std::string script, const std::string name, const eReturnType type) {
	set (script, name, type);
}

/** Destructor */
Script::~Script() {
	// TODO Auto-generated destructor stub
}

void Script::set (const std::string script, const std::string name, const eReturnType returnType) {
	this -> source = script;
	this -> name = name;
	this -> returnType = returnType;
}

void Script::clear() {
	error = "";
	result = "";
}

/** Getter/Setter for source */
std::string Script::getSource() const {
	return source;
}

std::string Script::getExecutableSource() const {
	return source;
}

void Script::setSource (const std::string script) {
	source = script;
	clear();
}

/** Getter/Setter for script return type */
Script::eReturnType Script::getReturnType() const {
	return returnType;
}

void Script::setReturnType(const Script::eReturnType type) {
	returnType = type;
}

/** Was there an error loading this script ? */
bool Script::hasError() const {
	return error.length() > 0;
}

void Script::setError (const std::string error) {
	this -> error = error;
}

/** Return any error */
std::string Script::getError() const {
	return error;
}

void Script::setName (const std::string name) {
	this -> name = name;
}

std::string Script::getName() const {
	return name;
}

void Script::setResult (const std::string result) {
	this -> result = result;
}

std::string Script::getResult() const {
	return result;
}


/**
 * Evaluate the script using the supplied context.  VassalContext provides a getProperty(name) call
 * that will be used by the evaluation environment to evaluate Vassal properties referenced in the script
 */
bool Script::execute (Scriptable *context) {

	clear();

	// Execute the script
	ScriptingFactory::getInstance() -> getEnvironment() -> execute(this, context);

	if (! hasError()) {
		return true;
	}

	return false;
}

/** Validate the script for syntactic correctness. Do not attempt to evaluate it, or save any reference to it.
 * */
bool Script::validate () {

	// Validate the script
	ScriptingFactory::getInstance() -> getEnvironment() -> validate (this);
	if (hasError()) {
		return false;
	}

	// Just return true if it succeeds validation
	return true;

}
