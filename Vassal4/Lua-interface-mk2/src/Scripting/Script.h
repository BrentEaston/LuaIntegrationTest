/*
 * Script.h
 *
 *  Created on: 31/07/2017
 *      Author: Brent
 */

#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <string>

class Scriptable;

class Script {
public:

	enum eReturnType {eReturnType_Invalid, eReturnType_Int, eReturnType_Float, eReturnType_String, eReturnType_None };

	Script(const std::string script);
	Script(const std::string script, const std::string name);
	Script(const std::string script, const std::string name, const eReturnType type);
	Script(const std::string script, const eReturnType type);
	virtual ~Script();

	std::string getSource() const;
	virtual std::string getExecutableSource() const;
	void setSource (const std::string script);

	eReturnType getReturnType() const;
	void setReturnType(const eReturnType type);

	bool execute (Scriptable *context);
	bool validate ();

	void setResult (const std::string result);
	std::string getResult() const;

	void setName (const std::string name);
	std::string getName() const;

	bool hasError() const;
	std::string getError() const;
	void setError (const std::string error);

protected:

	void set (const std::string script, const std::string name, const eReturnType returnType);
	void clear();

	std::string name;
	std::string source;
	eReturnType returnType;
	std::string result;
	std::string error;
};

#endif /* SCRIPT_H_ */
