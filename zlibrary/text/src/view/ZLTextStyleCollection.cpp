/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <stdlib.h>
#include <string.h>

#include <ZLibrary.h>
#include <ZLXMLReader.h>

#include "ZLTextStyle.h"

ZLTextStyleCollection *ZLTextStyleCollection::ourInstance = 0;

ZLTextStyleCollection &ZLTextStyleCollection::instance() {
	if (ourInstance == 0) {
		ourInstance = new ZLTextStyleCollection();
	}
	return *ourInstance;
}

void ZLTextStyleCollection::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
		ourInstance = 0;
	}
}

class ZLTextStyleReader : public ZLXMLReader {

public:
	ZLTextStyleReader(ZLTextStyleCollection &collection) : myCollection(collection) {}

	void startElementHandler(const char *tag, const char **attributes);

private:
	static int intValue(const char **attributes, const char *name, int defaultValue = 0);
	static bool booleanValue(const char **attributes, const char *name);
	static ZLBoolean3 b3Value(const char **attributes, const char *name);

private:
	ZLTextStyleCollection &myCollection;
};

static const std::string TRUE_STRING = "true";

inline int ZLTextStyleReader::intValue(const char **attributes, const char *name, int defaultValue) {
	const char *stringValue = attributeValue(attributes, name);
	return (stringValue == 0) ? defaultValue : atoi(stringValue);
}

inline bool ZLTextStyleReader::booleanValue(const char **attributes, const char *name) {
	const char *stringValue = attributeValue(attributes, name);
	return (stringValue != 0) && (TRUE_STRING == stringValue);
}

inline ZLBoolean3 ZLTextStyleReader::b3Value(const char **attributes, const char *name) {
	const char *stringValue = attributeValue(attributes, name);
	return (stringValue == 0) ? B3_UNDEFINED : ((TRUE_STRING == stringValue) ? B3_TRUE : B3_FALSE);
}

void ZLTextStyleReader::startElementHandler(const char *tag, const char **attributes) {
	static const std::string BASE = "base";
	static const std::string STYLE = "style";

	if (BASE == tag) {
		myCollection.myBaseStyle = new ZLTextBaseStyle(attributeValue(attributes, "family"), intValue(attributes, "fontSize"));
	} else if (STYLE == tag) {
		const char *idString = attributeValue(attributes, "id");
		const char *name = attributeValue(attributes, "name");
		if ((idString != 0) && (name != 0)) {
			ZLTextKind id = (ZLTextKind)atoi(idString);
			ZLTextStyleDecoration *decoration;

			int fontSizeDelta = intValue(attributes, "fontSizeDelta");
			ZLBoolean3 bold = b3Value(attributes, "bold");
			ZLBoolean3 italic = b3Value(attributes, "italic");
			int verticalShift = intValue(attributes, "vShift");
			ZLBoolean3 allowHyphenations = b3Value(attributes, "allowHyphenations");
			ZLTextStyleDecoration::HyperlinkStyle hyperlinkStyle = ZLTextStyleDecoration::NONE;
			const char *hyperlink = attributeValue(attributes, "hyperlink");
			if (hyperlink != 0) {
				static const std::string INTERNAL_STRING = "internal";
				if (INTERNAL_STRING == hyperlink) {
					hyperlinkStyle = ZLTextStyleDecoration::INTERNAL;
				}
				static const std::string EXTERNAL_STRING = "external";
				if (EXTERNAL_STRING == hyperlink) {
					hyperlinkStyle = ZLTextStyleDecoration::EXTERNAL;
				}
			}

			if (booleanValue(attributes, "partial")) {
				decoration = new ZLTextStyleDecoration(name, fontSizeDelta, bold, italic, verticalShift, allowHyphenations);
			} else {
				int spaceBefore = intValue(attributes, "spaceBefore");
				int spaceAfter = intValue(attributes, "spaceAfter");
				int leftIndent = intValue(attributes, "leftIndent");
				int rightIndent = intValue(attributes, "rightIndent");
				int firstLineIndentDelta = intValue(attributes, "firstLineIndentDelta");

				ZLTextAlignmentType alignment = ALIGN_UNDEFINED;
				const char *alignmentString = attributeValue(attributes, "alignment");
				if (alignmentString != 0) {
					if (strcmp(alignmentString, "left") == 0) {
						alignment = ALIGN_LEFT;
					} else if (strcmp(alignmentString, "rigth") == 0) {
						alignment = ALIGN_RIGHT;
					} else if (strcmp(alignmentString, "center") == 0) {
						alignment = ALIGN_CENTER;
					} else if (strcmp(alignmentString, "justify") == 0) {
						alignment = ALIGN_JUSTIFY;
					}
				}
				const int lineSpacingPercent = intValue(attributes, "lineSpacingPercent", -1);
				const double lineSpace = (lineSpacingPercent == -1) ? 0.0 : (lineSpacingPercent / 100.0);

				decoration = new ZLTextFullStyleDecoration(name, fontSizeDelta, bold, italic, spaceBefore, spaceAfter, leftIndent, rightIndent, firstLineIndentDelta, verticalShift, alignment, lineSpace, allowHyphenations);
			}
			decoration->setHyperlinkStyle(hyperlinkStyle);

			const char *fontFamily = attributeValue(attributes, "family");
			if (fontFamily != 0) {
				decoration->FontFamilyOption.setValue(fontFamily);
			}

			myCollection.myDecorationMap.insert(std::pair<ZLTextKind,ZLTextStyleDecoration*>(id, decoration));
		}
	}
}

ZLTextStyleCollection::ZLTextStyleCollection() :
	myBaseStyle(0) {
	ZLTextStyleReader(*this).readDocument(ZLibrary::DefaultFilesPathPrefix() + "styles.xml");
	if (myBaseStyle.isNull()) {
		myBaseStyle = new ZLTextBaseStyle("", 20);
	}
}

ZLTextStyleCollection::~ZLTextStyleCollection() {
	for (std::map<ZLTextKind,ZLTextStyleDecoration*>::iterator it = myDecorationMap.begin(); it != myDecorationMap.end(); ++it) {
		delete (*it).second;
	}
}

ZLTextStyleDecoration *ZLTextStyleCollection::decoration(ZLTextKind kind) const {
	std::map<ZLTextKind,ZLTextStyleDecoration*>::const_iterator it = myDecorationMap.find(kind);
	return (it != myDecorationMap.end()) ? (*it).second : 0;
}
