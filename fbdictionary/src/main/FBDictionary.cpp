/*
 * FBDictionary -- multi-format dictionary viewer
 * Copyright (C) 2007 Nikolay Pultsin <geometer@fbreader.org>
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

#include <ZLTextModel.h>
#include <ZLOptionEntry.h>

#include "FBDictionary.h"

DictionaryView::DictionaryView(ZLApplication &application, ZLPaintContext &context) : ZLTextView(application, context) {
}

const std::string &DictionaryView::caption() const {
	static const std::string EMPTY = "";
	return EMPTY;
}

class WordEntry : public ZLStringOptionEntry {

public:
	WordEntry(FBDictionary &fbdictionary) : myDictionary(fbdictionary) {}

private:
	const std::string &initialValue() const { static const std::string hello = "HELLO"; return hello; }
	void onAccept(const std::string &value) {}
	bool useOnValueEdited() const { return true; }
	void onValueEdited(const std::string &value) { myDictionary.openWord(value); }

private:
	FBDictionary &myDictionary;
};

FBDictionary::FBDictionary() : ZLApplication("FBDictionary"), myBindings("Keys") {
	myDictionaryView = new DictionaryView(*this, context());
	setView(myDictionaryView);

	addAction(FBAction::SHOW_OPTIONS_DIALOG, new ShowOptionsDialogAction(*this));

	toolbar().addButton(FBAction::SHOW_OPTIONS_DIALOG, ZLResourceKey("settings"));
	toolbar().addOptionEntry(new WordEntry(*this));
}

FBDictionary::~FBDictionary() {
}

void FBDictionary::openWord(const std::string &word) {
	ZLTextPlainModel *model = new ZLTextPlainModel();
	model->createParagraph(ZLTextParagraph::TEXT_PARAGRAPH);
	model->addControl(KIND_WORD, true);
	model->addText(word);
	model->addControl(KIND_WORD, false);
	model->addText(" ");
	model->addControl(KIND_PRONOUNCATION, true);
	model->addText("[pronouncation]");
	model->addControl(KIND_PRONOUNCATION, false);
	model->createParagraph(ZLTextParagraph::TEXT_PARAGRAPH);
	model->addControl(KIND_DEFINITION, true);
	model->addText("1. definition number one");
	model->addControl(KIND_DEFINITION, false);
	model->createParagraph(ZLTextParagraph::TEXT_PARAGRAPH);
	model->addControl(KIND_DEFINITION, true);
	model->addText("2. definition number two");
	model->addControl(KIND_DEFINITION, false);
	((ZLTextView&)*myDictionaryView).setModel(model, "");
	refreshWindow();
}

ZLKeyBindings &FBDictionary::keyBindings() {
	return myBindings;
}
