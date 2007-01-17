/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

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
