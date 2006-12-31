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
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, int len);
	ObjectPtr createObject(const ObjectInfoMap &info) const;

private:
	shared_ptr<Scene> myScene;
	ObjectEnumerator myEnumerator;
	int myId;
	int myPersistence;
	bool myReadDescription;
};

#endif /* __SCENEREADER_H__ */
