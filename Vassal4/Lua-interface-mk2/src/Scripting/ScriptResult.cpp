/*
 * ScriptResult.cpp
 *
 *  Created on: 17 Aug. 2017
 *      Author: Brent
 */

#include <Scripting/ScriptResult.h>

ScriptResult::ScriptResult() {
	resultLevel = eResult_Success;
	result = std::make_unique<TValue>();
	error = "";

}

ScriptResult::~ScriptResult() {
	// TODO Auto-generated destructor stub
}

bool ScriptResult::isSuccessful() const {
	return getResultLevel() == eResult_Success;
}

int ScriptResult::getResultLevel() const {
	return resultLevel;
}

const std::unique_ptr<TValue> & ScriptResult::getResult() const {
	return result;
}

void ScriptResult::setResultValue (std::unique_ptr<TValue> result) {
	this->result = std::move(result);
	setResultLevel(eResult_Success);
}

std::string ScriptResult::getError() const {
	return error;
}

void ScriptResult::setScriptError (const std::string error) {
	setError (error, eResult_Script_Error);
}

void ScriptResult::setInterfaceError (const std::string error) {
	setError (error, eResult_Interface_Error);
}

void ScriptResult::setVassalError (const std::string error) {
	setError (error, eResult_Vassal_Error);
}

void ScriptResult::setError (const std::string error, const ScriptResult::eResult errorLevel) {
	this -> error = error;
	setResultLevel(errorLevel);
}

void ScriptResult::setResultLevel (const ScriptResult::eResult level) {
	this -> resultLevel = level;
}
