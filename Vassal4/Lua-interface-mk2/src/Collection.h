/*
 * Collection.h
 *
 *  Created on: 27 Aug. 2017
 *      Author: Brent
 */

#ifndef COLLECTION_H_
#define COLLECTION_H_

#include <Scripting/Scriptable.h>
#include <vector>

using namespace std;

class Collection : public Scriptable {
public:
	Collection();
	virtual ~Collection();

	void addItem(Scriptable *item);
	Scriptable *getItem(int position) const;

	int size() const;

	TValue *get(const std::string propertyName) const;
	void set (const string id, TValue value);
	TValue *properties() const;

	eType getScriptableType () const;


private:
	vector<Scriptable*> contents;
};

#endif /* COLLECTION_H_ */
