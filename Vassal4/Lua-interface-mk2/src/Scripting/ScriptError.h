/*
 * ScriptError.h
 *
 *  Created on: 31 Aug. 2017
 *      Author: Brent
 */

#ifndef SCRIPTING_SCRIPTERROR_H_
#define SCRIPTING_SCRIPTERROR_H_

#include <Scripting/ScriptResult.h>

class ScriptError {
public:
	ScriptError(const string &message, ScriptResult::eResult level, const string &name, const string &info);
	ScriptError();
	virtual ~ScriptError();

	virtual const string formatError() const;

	virtual const ScriptResult::eResult getLevel() const;
	virtual const string &getMessage() const;
	virtual const string &getSource() const;
	virtual const string &getInfo() const;

protected:
	virtual void setLevel(const ScriptResult::eResult errorLevel);
	virtual void setMessage(const string &errorMessage);
	virtual void setSource(const string &sourceName);
	virtual void setInfo(const string &info);

private:
	ScriptResult::eResult level;
	string message;
	string source;
	string info;
};

#endif /* SCRIPTING_SCRIPTERROR_H_ */
