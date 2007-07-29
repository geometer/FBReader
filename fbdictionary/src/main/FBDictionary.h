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

#ifndef __FBDICTIONARY_H__
#define __FBDICTIONARY_H__

#include <ZLTextView.h>
#include <ZLApplication.h>
#include <ZLKeyBindings.h>

enum FBDTextKind {
	KIND_WORD = 1,
	KIND_PRONOUNCATION = 2,
	KIND_DEFINITION = 3,
};

class DictionaryView : public ZLTextView {

public:
	DictionaryView(ZLApplication &application, ZLPaintContext &context);

	const std::string &caption() const;
};

class FBDictionary : public ZLApplication { 

public:
	FBDictionary();
	~FBDictionary();

	ZLKeyBindings &keyBindings();

	void openWord(const std::string &word);

private:
	ZLKeyBindings myBindings;
	shared_ptr<ZLView> myDictionaryView;
};

class FBAction : public ZLApplication::Action {

public:
	enum Code {
		SHOW_OPTIONS_DIALOG = 1,
	};

protected:
	FBAction(FBDictionary &fbdictionary);
	FBDictionary &fbdictionary();

private:
	FBDictionary &myFBDictionary;
};

class ShowOptionsDialogAction : public FBAction {

public:
	ShowOptionsDialogAction(FBDictionary &fbdictionary);
	void run();
};

inline FBAction::FBAction(FBDictionary &fbdictionary) : myFBDictionary(fbdictionary) {}
inline FBDictionary &FBAction::fbdictionary() { return myFBDictionary; }

#endif /* __FBDICTIONARY_H__ */
