#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLInputStream.h>

#include "SceneSetNameReader.h"

std::string SceneSetNameReader::readSetName(const ZLFile &file) {
	shared_ptr<ZLDir> archiveDir = file.directory();
	mySetName = "";
	if (!archiveDir.isNull()) {
		ZLFile descriptionFile(archiveDir->itemName("description.xml"));
		readDocument(descriptionFile.inputStream());
	}
	return mySetName;
}

void SceneSetNameReader::startElementHandler(const char *tag, const char **attributes) {
	if (std::string("sceneSet") == tag) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			mySetName = name;
		}
	}

	interrupt();
}
