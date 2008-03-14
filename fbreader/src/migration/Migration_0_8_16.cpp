/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLUnicodeUtil.h>
#include <ZLFile.h>

#include "Migration.h"
#include "FB2MigrationReader.h"
#include "OEBMigrationReader.h"
#include "HtmlDCTagsReader.h"

#include "../options/FBOptions.h"
#include "../description/BookDescription.h"
#include "../formats/oeb/OEBPlugin.h"
#include "../formats/pdb/PdbPlugin.h"
#include "../formats/pdb/MobipocketStream.h"

Migration_0_8_16::Migration_0_8_16() : Migration("0.8.16") {
}

void Migration_0_8_16::doMigrationInternal() {
	PluginCollection &collection = PluginCollection::instance();

	std::vector<std::string> optionGroups;
	ZLOption::listOptionGroups(optionGroups);

	for (std::vector<std::string>::const_iterator it = optionGroups.begin(); it != optionGroups.end(); ++it) {
		if (isLikeToFileName(*it)) {
			ZLFile file(*it);
			if (collection.plugin(file, false) != 0) {
				BookInfo info(*it);
				ZLStringOption &languageOption = info.LanguageOption;
				const std::string &language = languageOption.value();
				if (language == "") {
					languageOption.setValue(PluginCollection::instance().DefaultLanguageOption.value());
				} else if (language == "cz") {
					languageOption.setValue("cs");
				} else if (language == "none") {
					languageOption.setValue("other");
				} else if ((language == "chinese") || (language == "anycharacter")) {
					languageOption.setValue("zh");
				}

				const std::string extension = ZLUnicodeUtil::toLower(file.extension());
				if (extension == "fb2") {
					ZLBooleanOption seriesOption(FBCategoryKey::BOOKS, *it, "SequenceDefined", false);
					if (!seriesOption.value() || info.TagsOption.value().empty()) {
						FB2MigrationReader(info, !seriesOption.value()).doRead(*it);
					}
					seriesOption.setValue(true);
				} else if ((extension == "opf") || (extension == "oebzip") || (extension == "epub")) {
					if (info.TagsOption.value().empty()) {
						OEBMigrationReader(info).doRead(OEBPlugin::opfFileName(*it));
					}
				} else if ((extension == "prc") || (extension == "pdb") || (extension == "mobi")) {
					const std::string fileType = PdbPlugin::fileType(file);
					if (info.TagsOption.value().empty() && ((fileType == "BOOKMOBI") || (fileType == "TEXtREAd"))) {
						shared_ptr<ZLInputStream> stream = new MobipocketStream(file);
						if (!stream.isNull()) {
							HtmlDCTagsReader(info).readDocument(*stream);
						}
					}
				}
			}
		}
	}
}
