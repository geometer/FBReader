#include <set>

#include <ZLStringUtil.h>

#include "../model/ObjectUtil.h"
#include "SceneWriter.h"

SceneWriter::SceneWriter(ZLOutputStream &textStream) : ZLXMLWriter(textStream) {
}

void SceneWriter::write(const Scene &scene, const std::string &sceneName) {
	std::set<ObjectPtr> persistentObjects;
	const std::list<ObjectPtr> &objects = scene.objects();
	persistentObjects.insert(objects.begin(), objects.end());

	std::list<ObjectPtr> closure = ObjectUtil::orderedClosure(objects);
	ObjectEnumerator enumerator;

	addTag("scene", false);
	addAttribute("name", sceneName);
	if (!scene.description().empty()) {
		addTag("description", false);
		addData(scene.description());
		closeTag();
	}
	for (std::list<ObjectPtr>::const_iterator jt = closure.begin(); jt != closure.end(); jt++) {
		enumerator.addObject(*jt);
		addTag("object", false);

		std::string idString;
		ZLStringUtil::appendNumber(idString, enumerator.id(*jt));
		addAttribute("id", idString);
		if (persistentObjects.find(*jt) != persistentObjects.end()) {
			addAttribute("persistent", "true");
		}
		addTag("data", true);
		ObjectInfoMap info(enumerator);
		(*jt)->saveInfo(info);
		const std::map<std::string,std::string> &data = info.data();
		for (std::map<std::string,std::string>::const_iterator kt = data.begin(); kt != data.end(); ++kt) {
			addAttribute(kt->first, kt->second);
		}
		closeTag();
	}
	closeTag();
}
