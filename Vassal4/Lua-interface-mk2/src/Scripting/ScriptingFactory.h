/*
 * ScriptingFactory.h
 *
 *  Created on: 01/08/2017
 *      Author: Brent
 */

#ifndef SCRIPTINGFACTORY_H_
#define SCRIPTINGFACTORY_H_

#include <memory>

class ScriptingEnvironment;

class ScriptingFactory {
public:

	enum SupportedLanguages {eLua, eJavascript, ePython };

	static ScriptingFactory* getInstance();

	std::unique_ptr<ScriptingEnvironment>& getEnvironment();

protected:

	SupportedLanguages language;
	std::unique_ptr<ScriptingEnvironment> environment;

private:
	static ScriptingFactory* instance;

	ScriptingFactory(); //prevent construction from outside this class' methods
	ScriptingFactory(const ScriptingFactory&) = delete;
	ScriptingFactory& operator=(const ScriptingFactory&) = delete;
     ~ScriptingFactory() = default;

};

#endif /* SCRIPTINGFACTORY_H_ */
