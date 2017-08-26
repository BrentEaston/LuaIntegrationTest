/*
 * ScriptingEnvironment.h
 *
 *  Created on: 01/08/2017
 *      Author: Brent
 */

#ifndef SCRIPTINGENVIRONMENT_H_
#define SCRIPTINGENVIRONMENT_H_

#include <string>

class ScriptResult;

class Script;
class Scriptable;

/*!
 * \class ScriptingEnvironment
 *
 * \brief Base class for language specific scripting environments
 *
 * ScriptingEnvironment provides the basic interface that any language specific implementation must follow.
 *
 * \author Brent Easton
 * \date 3-Aug-17
 */
class ScriptingEnvironment {
public:
	virtual ~ScriptingEnvironment() {};

	/*!
	 * \brief Has the Scripting Environment been successfully initialised?
	 */
	virtual const bool isInitialised() const=0;

	/*!
	 * \brief Has the Scripting Environment been successfully initialised?
	 */
	virtual const std::string getInitialisationError() const=0;

	/*!
	 * \brief Validate an expression
	 *
	 * Validate the supplied expression but do not load it permanently into the scripting environment
	 *
	 * \param in expression The expression to be validated
	 * \param out error Error message if validation fails
	 * \return boolean set to true if \a expression passed validation
	 */
	virtual void validate (Script *script, ScriptResult &result)=0;

	/*!
	 * \brief Execute a script
	 *
	 * Evaluate a previously loaded expression
	 *
	 * \param in handle The ExpressionHandle that identifies the compiled expression
	 * \param out result The result of the expression evaluation
	 * \param out error Error message if there was a run-time error during evaluation
	 * \return boolean set to true if there was no run-time error
	 *
	 */
	virtual void execute (Script *script, const Scriptable *context, ScriptResult &result)=0;

protected:
	bool initialised;
	std::string initialisationError;
};

#endif /* SCRIPTINGENVIRONMENT_H_ */
