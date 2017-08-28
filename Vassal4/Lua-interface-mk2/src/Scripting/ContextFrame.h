/*
 * ContextFrame.h
 *
 *  Created on: 15 Aug. 2017
 *      Author: Brent
 */

#ifndef CONTEXTFRAME_H_
#define CONTEXTFRAME_H_

#include <Scripting/ProxyFactory.h>
#include <Scripting/Scriptable.h>
#include <memory>
#include <unordered_map>

/**
 * A ContextFrame is associated with each invocation of a script. It contains the Proxy objects that talk to the Vassal objects for all
 * Vassal objects referenced by the script. Script execution is full re-entrant. If a script calls itself, either directly or indirectly,
 * a new ContextFrame will be allocated with a new set of Proxy objects.
 */
class ContextFrame {
public:
	ContextFrame();
	ContextFrame(const Scriptable *thisPtr);
	ContextFrame(const unique_ptr<Scriptable> thisPtr);
	virtual ~ContextFrame();

	/** Create, or find an existing Proxy for a given Scriptable supplied by Vassal. Scriptables are generally provided to us and are owned by Vassal,
	 * hence the shared_ptr.
	 */
	std::unique_ptr<Proxy> const & getProxy(const Scriptable *scriptable);

	/** Create, or find an existing Proxy for a raw scriptable pointer and type supplied from the user script.
	 *
	 */
	std::unique_ptr<Proxy> const & getProxy(Scriptable::eType type, const void *ptr);

private:
	/** A map of all Proxy objects used by the script invocation this frame is attached to.
	 * The ContextFrame for an invocation owns the proxies, hence using unique_ptr
	 * The map is keyed on the raw address of the proxy.
	 */
	std::unordered_map<const void *, std::unique_ptr<Proxy>> proxies;

	/** A factory to create Proxies to wrap Scriptables */
	ProxyFactory proxyFactory;
};

#endif /* CONTEXTFRAME_H_ */
