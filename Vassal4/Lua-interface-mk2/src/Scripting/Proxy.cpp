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

	// Check there is a proxy definition
	if (proxyDefinitions.count(proxyName) == 0) {
		result.setInterfaceError("No Proxy Definition found for " + proxyName);
		return;
	}

	// Check the operation is valid.
	if (!proxyDefinitions[proxyName]->isOperationValid(operationName)) {
		result.setInterfaceError("Invalid operation " + proxyName + ":" + operationName + ".");
		return;
	}

	ProxyOperation *operation = proxyDefinitions[proxyName]->getOperation(
			operationName).get();

	// Check the number of Arguments matches.
	if (((int) args.size()) != operation->getArgumentCount()) {
		result.setInterfaceError("Invalid number of arguments for " + proxyName + ":"
				+ operationName + ". Expected "
				+ to_string(operation->getArgumentCount()) + ", got "
				+ to_string(args.size()));
		return;
	}

	// Check the types of Arguments matches
	for (int argIndex = 0; argIndex < ((int) args.size()); argIndex++) {
		TValue::eType expectedArgType = operation->getArgumentType(argIndex);
		TValue::eType actualArgType = args.at(argIndex)->getType();
		if (expectedArgType != actualArgType) {
			result.setInterfaceError("Invalid type for argument " + to_string(argIndex + 1)
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

