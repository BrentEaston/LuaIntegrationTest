/*
 * ScriptError.cpp
 *
 *  Created on: 31 Aug. 2017
 *      Author: Brent
 */

#include <Scripting/ScriptError.h>
#include <iostream>
#include <string>

using namespace std;

ScriptError::ScriptError(const string &message, ScriptResult::eResult level, const string &source, const string &info) {
	setMessage (message);
	setLevel(level);
	setSource (source);
	setInfo (info);
}

ScriptError::ScriptError() {
	setMessage("");
	setLevel(ScriptResult::eResult_Success);
	setSource("");
	setInfo("");
}

ScriptError::~ScriptError() {
	// TODO Auto-generated destructor stub
}

const string ScriptError::formatError() const {
	return source+" "+to_string(level)+" "+message+" "+info;
}

const ScriptResult::eResult ScriptError::getLevel() const {
	return level;
}

const string &ScriptError::getMessage() const {
	return message;
}

const string &ScriptError::getSource() const {
	return source;
}

const string &ScriptError::getInfo() const {
	return info;
}

void ScriptError::setLevel(const ScriptResult::eResult errorLevel) {
	level = errorLevel;
}

void ScriptError::setMessage(const string &errorMessage) {
	message = errorMessage;
}

void ScriptError::setSource(const string &sourceName) {
	source = sourceName;
}

void ScriptError::setInfo(const string &info) {
	//cout << "ScriptError::setInfo: Info set to " << info << endl;
	this->info = info;
}


