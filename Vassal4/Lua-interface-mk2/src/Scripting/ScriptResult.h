/*
 * ScriptResult.h
 *
 *  Created on: 17 Aug. 2017
 *      Author: Brent
 */

#ifndef SCRIPTING_SCRIPTRESULT_H_
#define SCRIPTING_SCRIPTRESULT_H_

#include <Scripting/TValue.h>
#include <memory>
#include <string>

class ScriptResult {
public:

	enum eResult {
		eResult_Success,
		eResult_Script_Error,
		eResult_Vassal_Error
	};
	ScriptResult();
	virtual ~ScriptResult();

	bool isSuccessful() const;
	bool isScriptError() const;
	bool isVassalError() const;

	ScriptResult::eResult getResultLevel() const;
	const std::unique_ptr<TValue> & getResult() const;

	void setResultValue (std::unique_ptr<TValue> result);

	void setError (const std::string error, const ScriptResult::eResult errorLevel);
	void setScriptError (const std::string error);
	void setVassalError (const std::string error);
	std::string getError() const;
	void clearError();
protected:
	void setResultLevel (const ScriptResult::eResult level);

private:
	ScriptResult::eResult resultLevel;
	std::unique_ptr<TValue> result;
	std::string error;
};

#endif /* SCRIPTING_SCRIPTRESULT_H_ */
