#ifndef __SCENENAMEREADER_H__
#define __SCENENAMEREADER_H__

#include <ZLXMLReader.h>

class ZLFile;

class SceneNameReader : public ZLXMLReader {

public:
	std::string readSceneName(const ZLFile &file);

private:	
	void startElementHandler(const char *tag, const char **attributes);

private:
	std::string mySceneName;
};

#endif /* __SCENENAMEREADER_H__ */
