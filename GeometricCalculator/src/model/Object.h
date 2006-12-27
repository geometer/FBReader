#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <list>
#include <map>
#include <string>

#include <shared_ptr.h>

class Object;
typedef shared_ptr<Object> ObjectPtr;
typedef weak_ptr<Object> ObjectWeakPtr;
class PointPtr;
class ObjectInfoMap;
class Polynom;
class Translator;

struct Coordinates {
	bool isReal;
	double x, y;
	Coordinates(double x, double y, bool isReal) { this->x = x; this->y = y; this->isReal = isReal; }
	Coordinates(const Coordinates &c) { this->x = c.x; this->y = c.y; this->isReal = c.isReal; }
};



class Object {

public:
	enum Type {
		POINT = 1,
		LINE = 2,
		CIRCLE = 3,
	};

public:
	static const std::string TYPE_KEY;

	virtual ~Object() {}

	virtual void saveInfo(ObjectInfoMap &info) const = 0;

	virtual Type rtti() const = 0;

	virtual bool isReal() const;

	virtual double distance(double x, double y) const = 0;
	virtual double distance(const Coordinates coords) const = 0;
	virtual double distance(PointPtr point) const;

	virtual const std::list<ObjectPtr> &dependsOn() const;

	virtual std::list<Polynom> polynoms(Translator &translator) const = 0;
};

class ObjectEnumerator {

public:
	ObjectEnumerator();

	void addObject(ObjectPtr object, int id = -1);
	void clear();

	int id(ObjectPtr object) const;
	ObjectPtr object(int id) const;

private:
	std::map<ObjectPtr,int> myObjectToIdMap;
	std::map<int,ObjectPtr> myIdToObjectMap;

private:
	ObjectEnumerator(const ObjectEnumerator &oe);
	const ObjectEnumerator &operator = (const ObjectEnumerator &oe);
};

class ObjectInfoMap {

public:
	ObjectInfoMap(ObjectEnumerator &enumerator);

	void addAttribute(const std::string &name, const std::string &value);
	void addAttribute(const std::string &name, int value);
	void addAttribute(const std::string &name, double value);
	void addAttribute(const std::string &name, bool value);
	void addAttribute(const std::string &name, ObjectPtr value);

	const std::string &getStringValue(const std::string &name) const;
	bool stringValueEqualsTo(const std::string &name, const std::string &value) const;

	int getIntValue(const std::string &name) const;
	double getDoubleValue(const std::string &name) const;
	bool getBooleanValue(const std::string &name) const;
	ObjectPtr getObjectValue(const std::string &name) const;

	const std::map<std::string,std::string> &data() const;

private:
	ObjectEnumerator &myEnumerator;
	std::map<std::string,std::string> myAttributes;
};

#endif /* __OBJECT_H__ */
