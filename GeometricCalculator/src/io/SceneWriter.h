#ifndef __SCENEWRITER_H__
#define __SCENEWRITER_H__

#include <ZLOutputStream.h>
#include <ZLXMLWriter.h>

#include "../model/Scene.h"

class SceneWriter : public ZLXMLWriter {

public:
	SceneWriter(ZLOutputStream &textStream);

	void write(const Scene &scene, const std::string &sceneName);
};

#endif /* __SCENEWRITER_H__ */
