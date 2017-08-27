#include <Scripting/Proxy.h>
#include <Scripting/ProxyOperation.h>
#include <Scripting/ScriptResult.h>

using namespace std;

// Allocate the static variables for this class
unordered_map<string, unique_ptr<ProxyDefinition>> Proxy::proxyDefinitions;

Proxy::~Proxy() {
}

bool Proxy::isRegistered(string proxyName) {
	return proxyDefinitions.count(proxyName) > 0;
}

void Proxy::registerProxy(unique_ptr<ProxyDefinition> &proxyDefinition) {
	proxyDefinitions[proxyDefinition->getName()] = move(proxyDefinition);
}

void Proxy::checkOperationArguments(const string proxyName,
		const string operationName, vector<unique_ptr<TValue>> &args,
		ScriptResult &result) {

	// Check the Vassal object has not been destroyed
	if (isDestroyed()) {
		result.setScriptError("Attempt to perform" + operationName +" on destroyed object: "+ proxyName);
		return;
	}

	// Check there is a proxy definition
	if (proxyDefinitions.count(proxyName) == 0) {
		result.setVassalError("No Proxy Definition found for " + proxyName);
		return;
	}

	// TODO Can an object have named scripts registered against it that can be called by user scripts??? How do we know about them here?
	//      Will need to ask the wrapped object (should be done in checkOperationArguments)

	// Check the operation is valid.
	if (!proxyDefinitions[proxyName]->isOperationValid(operationName)) {
		result.setVassalError("Invalid operation " + proxyName + ":" + operationName + ".");
		return;
	}

	ProxyOperation *operation = proxyDefinitions[proxyName]->getOperation(
			operationName).get();

	// Check the number of Arguments matches.
	if (((int) args.size()) != operation->getArgumentCount()) {
		result.setVassalError("Invalid number of arguments for " + proxyName + ":"
				+ operationName + ". Expected "
				+ to_string(operation->getArgumentCount()) + ", got "
				+ to_string(args.size()));
		return;
	}

	// Check the types of Arguments matches
	for (int argIndex = 0; argIndex < ((int) args.size()); argIndex++) {
		TValue::eType expectedArgType = operation->getArgumentType(argIndex);
		TValue::eType actualArgType = args.at(argIndex)->getType();
		if (expectedArgType != TValue::eType_any && (expectedArgType != actualArgType)) {
			result.setVassalError("Invalid type for argument " + to_string(argIndex + 1)
					+ " for " + proxyName + ":" + operationName + ". Expected "
					+ TValue::getTypeName(expectedArgType) + "("
					+ to_string(expectedArgType) + "), got "
					+ TValue::getTypeName(actualArgType) + " ("
					+ to_string(actualArgType) + ")");
			return;
		}
	}

	// All good

	return;
}

void Proxy::notifyDestroyed() {
	destroyed = true;
}

bool Proxy::isDestroyed() const {
	return destroyed;
}
