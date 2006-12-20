#ifndef __SCENEREADER_H__
#define __SCENEREADER_H__

#include <ZLXMLReader.h>

#include "../model/Scene.h"
#include "../model/Object.h"

class SceneReader : public ZLXMLReader {

public:
	shared_ptr<Scene> readScene(const std::string &fileName);

private:	
	void startElementHandler(const char *tag, const char **attributes);
	ObjectPtr createObject(const ObjectInfoMap &info) const;

private:
	shared_ptr<Scene> myScene;
	ObjectEnumerator myEnumerator;
	int myId;
	int myPersistence;
};

#endif /* __SCENEREADER_H__ */
