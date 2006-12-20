#include "SceneReader.h"
#include "../model/Scene.h"
#include "../model/Point.h"
#include "../model/Line.h"
#include "../model/Circle.h"

shared_ptr<Scene> SceneReader::readScene(const std::string &fileName) {
	myScene = new Scene();
	myId = -1;
	myEnumerator.clear();
	readDocument(fileName);
	return myScene;
}

void SceneReader::startElementHandler(const char *tag, const char **attributes) {
	static const std::string SCENE = "scene";
	static const std::string OBJECT = "object";
	static const std::string DATA = "data";
	static const std::string TRUE = "true";

	if (SCENE == tag) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			myScene->setName(name);
		}
	} else if (OBJECT == tag) {
		const char *idString = attributeValue(attributes, "id");
		const char *persistenceString = attributeValue(attributes, "persistent");
		myId = (idString != 0) ? atoi(idString) : -1;
		myPersistence = (persistenceString != 0) && (TRUE == persistenceString);
	} else if (DATA == tag) {
		if (myId >= 0) {
			ObjectInfoMap info(myEnumerator);
			for (; ((*attributes) != 0) && ((*(attributes + 1)) != 0); attributes += 2) {
				info.addAttribute(*attributes, std::string(*(attributes + 1)));
			}
			ObjectPtr object = createObject(info);
			if (!object.isNull()) {
				myEnumerator.addObject(object, myId);
				if (myPersistence) {
					myScene->add(object);
				}
			}
			myId = -1;
		}
	}
}

ObjectPtr SceneReader::createObject(const ObjectInfoMap &info) const {
	ObjectPtr object;
	if (object.isNull()) object = IndependentPoint::create(info);
	if (object.isNull()) object = PointOnTheLine::create(info);
	if (object.isNull()) object = PointOnTheCircle::create(info);
	if (object.isNull()) object = PointOnPerpendicular::create(info);
	if (object.isNull()) object = LinesCrossing::create(info);
	if (object.isNull()) object = LineAndCircleCrossing::create(info);
	if (object.isNull()) object = CirclesCrossing::create(info);
	if (object.isNull()) object = FourthParallelogramPoint::create(info);
	if (object.isNull()) object = LineByTwoPoints::create(info);
	if (object.isNull()) object = CircleByCenterAndPoint::create(info);
	if (object.isNull()) object = CircleCopy::create(info);
	return object;
}
