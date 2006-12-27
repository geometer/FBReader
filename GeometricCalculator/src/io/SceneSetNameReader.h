#ifndef __SCENESETNAMEREADER_H__
#define __SCENESETNAMEREADER_H__

#include <ZLXMLReader.h>

class ZLFile;

class SceneSetNameReader : public ZLXMLReader {

public:
	std::string readSetName(const ZLFile &file);

private:	
	void startElementHandler(const char *tag, const char **attributes);

private:
	std::string mySetName;
};

#endif /* __SCENESETNAMEREADER_H__ */
