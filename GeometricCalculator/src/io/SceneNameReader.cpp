#include <ZLFile.h>
#include <ZLInputStream.h>

#include "SceneNameReader.h"

std::string SceneNameReader::readSceneName(const ZLFile &file) {
	mySceneName = "Unknown Scene";
	readDocument(file.inputStream());
	return mySceneName;
}

void SceneNameReader::startElementHandler(const char *tag, const char **attributes) {
	if (std::string("scene") == tag) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			mySceneName = name;
		}
	}

	interrupt();
}
