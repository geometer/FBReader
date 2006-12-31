#ifndef __SCENE_H__
#define __SCENE_H__

#include <list>

#include "Object.h"
#include "Value.h"

class Scene {

public:
	Scene() {}
	virtual ~Scene() {}

	void setName(const std::string &name);
	const std::string &name() const;

	void setDescription(const std::string &description);
	const std::string &description() const;

	bool isEmpty() const;

	const std::list<ObjectPtr> &objects() const;
	virtual void add(const ObjectPtr object);
	virtual void remove(const ObjectPtr object);

	bool isPersistent(const ObjectPtr object) const;

	const std::list<ValuePtr> &values() const;
	virtual void add(const ValuePtr value);
	virtual void remove(const ValuePtr value);

	virtual std::list<ObjectPtr>::iterator find(const ObjectPtr object);
	virtual std::list<ObjectPtr>::const_iterator const_find(const ObjectPtr object) const;

private:
	uint maxUsedPointNumber();

protected:
	std::list<ObjectPtr> myObjects;
	std::list<ValuePtr> myValues;

private:
	std::string myName;
	std::string myDescription;

private:
	Scene(const Scene&);
	Scene &operator=(const Scene&);
};

#endif /* __SCENE_H__ */
