/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ZLOPTIONENTRY_H__
#define __ZLOPTIONENTRY_H__

#include <vector>
#include <string>

#include <abstract/ZLOptions.h>

enum ZLOptionKind {
	CHOICE,
	BOOLEAN,
	STRING,
	SPIN,
	COMBO,
	COLOR,
	UNKNOWN,
};

class ZLOptionEntry;

class OptionView {

public:
	OptionView(ZLOptionEntry *option);
	virtual ~OptionView();
	virtual void show() = 0;
	virtual void hide() = 0;

protected:
	ZLOptionEntry *myOption;
};

class ZLOptionEntry {

public:
	ZLOptionEntry() : myView(0), myIsVisible(true) {}
	virtual ~ZLOptionEntry() {}
	virtual ZLOptionKind kind() const = 0;
	virtual const std::string &name() const = 0;

	void setView(OptionView *view) { myView = view; }
	void show();
	void hide();
	bool isVisible() const { return myIsVisible; }

private:
	OptionView *myView;
	bool myIsVisible;
};

class ZLUserDefinedOptionEntry : public ZLOptionEntry {

public:
	ZLOptionKind kind() const { return UNKNOWN; }
	virtual OptionView *createView() = 0;
};

class ZLChoiceOptionEntry : public ZLOptionEntry {

public:
	ZLOptionKind kind() const { return CHOICE; }

	virtual const std::string &text(int index) const = 0;
	virtual int choiceNumber() const = 0;
	virtual int initialCheckedIndex() const = 0;
	virtual void onAccept(int index) const = 0;
};

class ZLStringOptionEntry : public ZLOptionEntry {

public:
	ZLOptionKind kind() const { return STRING; }

	virtual const std::string &initialValue() const = 0;
	virtual void onAccept(const std::string &value) const = 0;
};

class ZLSimpleStringOptionEntry : public ZLStringOptionEntry {

public:
	ZLSimpleStringOptionEntry(const std::string &name, const ZLStringOption &option) : myName(name), myOption(option) {}
	const std::string &name() const { return myName; }
	const std::string &initialValue() const { return myOption.value(); }
	void onAccept(const std::string &value) const { myOption.setValue(value); }

private:
	std::string myName;
	const ZLStringOption &myOption;
};

class ZLBooleanOptionEntry : public ZLOptionEntry {

public:
	ZLOptionKind kind() const { return BOOLEAN; }

	virtual bool initialState() const = 0;
	virtual void onAccept(bool state) const = 0;
};

class ZLSimpleBooleanOptionEntry : public ZLBooleanOptionEntry {

public:
	ZLSimpleBooleanOptionEntry(const std::string &name, const ZLBooleanOption &option) : myName(name), myOption(option) {}
	const std::string &name() const { return myName; }
	bool initialState() const { return myOption.value(); }
	void onAccept(bool state) const { myOption.setValue(state); }

private:
	std::string myName;
	const ZLBooleanOption &myOption;
};

class ZLSpinOptionEntry : public ZLOptionEntry {

public:
	ZLOptionKind kind() const { return SPIN; }

	virtual int initialValue() const = 0;
	virtual int minValue() const = 0;
	virtual int maxValue() const = 0;
	virtual int step() const = 0;
	virtual void onAccept(int value) const = 0;
};

class ZLSimpleSpinOptionEntry : public ZLSpinOptionEntry {

public:
	ZLSimpleSpinOptionEntry(const std::string &name, const ZLIntegerOption &option, int minValue, int maxValue, int step) : myName(name), myOption(option), myMinValue(minValue), myMaxValue(maxValue), myStep(step) {}
	const std::string &name() const { return myName; }
	int initialValue() const { return myOption.value(); }
	int minValue() const { return myMinValue; }
	int maxValue() const { return myMaxValue; }
	int step() const { return myStep; }
	void onAccept(int value) const { myOption.setValue(value); }

private:
	std::string myName;
	const ZLIntegerOption &myOption;
	int myMinValue;
	int myMaxValue;
	int myStep;
};

class ZLComboOptionEntry : public ZLOptionEntry {

public:
	ZLOptionKind kind() const { return COMBO; }

	virtual const std::string &initialValue() const = 0;
	virtual const std::vector<std::string> &values() const = 0;
	virtual void onValueChange(const std::string&) {}
	virtual void onAccept(const std::string &value) const = 0;
};

class ZLSimpleBoolean3OptionEntry : public ZLComboOptionEntry {

public:
	ZLSimpleBoolean3OptionEntry(const std::string &name, const ZLBoolean3Option &option) : myName(name), myOption(option) {}
	const std::string &name() const { return myName; }
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value) const;

private:
	static std::vector<std::string> ourValues;
	std::string myName;
	const ZLBoolean3Option &myOption;
};

class ZLColorOptionEntry : public ZLOptionEntry {

public:
	ZLOptionKind kind() const { return COLOR; }

	virtual const ZLColor color() const = 0;
	virtual void onAccept(ZLColor color) = 0;
};

class ZLSimpleColorOptionEntry : public ZLColorOptionEntry {

public:
	ZLSimpleColorOptionEntry(ZLColorOption &option) : myOption(option) {}
	const std::string &name() const;
	const ZLColor color() const { return myOption.value(); }
	void onAccept(ZLColor color) { myOption.setValue(color); }

private:
	ZLColorOption &myOption; 
};

#endif /* __ZLOPTIONENTRY_H__ */
