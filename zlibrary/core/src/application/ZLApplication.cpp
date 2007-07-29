/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005, 2006 Mikhail Sobolev <mss@mawhrin.net>
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

#include <ZLibrary.h>

#include "ZLApplication.h"
#include "ZLApplicationWindow.h"
#include "ZLKeyBindings.h"
#include "../view/ZLView.h"
#include "../view/ZLPaintContext.h"

const std::string ZLApplication::MouseScrollDownKey = "<MouseScrollDown>";
const std::string ZLApplication::MouseScrollUpKey = "<MouseScrollUp>";

static const std::string ROTATION = "Rotation";
static const std::string ANGLE = "Angle";
static const std::string STATE = "State";
static const std::string KEYBOARD = "Keyboard";
static const std::string FULL_CONTROL = "FullControl";
static const std::string CONFIG = "Config";
static const std::string AUTO_SAVE = "AutoSave";
static const std::string TIMEOUT = "Timeout";

ZLApplication::ZLApplication(const std::string &name) : ZLApplicationBase(name),
	RotationAngleOption(ZLOption::CONFIG_CATEGORY, ROTATION, ANGLE, ZLViewWidget::DEGREES90),
	AngleStateOption(ZLOption::CONFIG_CATEGORY, STATE, ANGLE, ZLViewWidget::DEGREES0),
	KeyboardControlOption(ZLOption::CONFIG_CATEGORY, KEYBOARD, FULL_CONTROL, false),
	ConfigAutoSavingOption(ZLOption::CONFIG_CATEGORY, CONFIG, AUTO_SAVE, true),
	ConfigAutoSaveTimeoutOption(ZLOption::CONFIG_CATEGORY, CONFIG, TIMEOUT, 1, 6000, 30),
	KeyDelayOption(ZLOption::CONFIG_CATEGORY, "Options", "KeyDelay", 0, 5000, 250),
	myViewWidget(0),
	myWindow(0) {
	myContext = ZLibrary::createContext();
	if (ConfigAutoSavingOption.value()) {
		ZLOption::startAutoSave(ConfigAutoSaveTimeoutOption.value());
	}
}

ZLApplication::~ZLApplication() {
	if (myWindow != 0) {
		if (KeyboardControlOption.value()) {
			grabAllKeys(false);
		}
		delete myWindow;
	}
	if (myViewWidget != 0) {
		delete myViewWidget;
	}
	delete myContext;
}

void ZLApplication::initWindow() {
	if (KeyboardControlOption.value()) {
		grabAllKeys(true);
	}
	myWindow->init();
	setView(myInitialView);
}

bool ZLApplication::closeView() {
	quit();
	return true;
}

void ZLApplication::openFile(const std::string&) {
}

ZLApplicationWindow::ZLApplicationWindow(ZLApplication *application) : myApplication(application), myToggleButtonLock(false) {
	myApplication->myWindow = this;
}

void ZLApplicationWindow::init() {
	myApplication->myViewWidget = createViewWidget();

	const ZLApplication::Toolbar::ItemVector &toolbarItems = myApplication->toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = toolbarItems.begin(); it != toolbarItems.end(); ++it) {
		addToolbarItem(*it);
	}

	initMenu();
}

void ZLApplicationWindow::onButtonPress(ZLApplication::Toolbar::ButtonItem &button) {
	if (myToggleButtonLock) {
		return;
	}
	if (button.isToggleButton()) {
		myToggleButtonLock = true;
		if (button.isPressed()) {
			setToggleButtonState(button);
			myToggleButtonLock = false;
			return;
		} else {
			button.press();
			shared_ptr<ZLApplication::Toolbar::ButtonGroup> group = button.buttonGroup();
			const std::set<const ZLApplication::Toolbar::ButtonItem*> &items = group->Items;
			for (std::set<const ZLApplication::Toolbar::ButtonItem*>::const_iterator it = items.begin(); it != items.end(); ++it) {
				setToggleButtonState(**it);
			}
		}
		myToggleButtonLock = false;
	}
	application().doAction(button.actionId());
}

void ZLApplication::addAction(int actionId, shared_ptr<Action> action) {
	myActionMap[actionId] = action;
}

void ZLApplication::setView(shared_ptr<ZLView> view) {
	if (view.isNull()) {
		return;
	}

	if (myViewWidget != 0) {
		myViewWidget->setView(view);
		resetWindowCaption();
		refreshWindow();
	} else {
		myInitialView = view;
	}
}

shared_ptr<ZLView> ZLApplication::currentView() const {
	return (myViewWidget != 0) ? myViewWidget->view() : 0;
}

void ZLApplication::refreshWindow() {
	if (myViewWidget != 0) {
		myViewWidget->repaint();
	}
	if (myWindow != 0) {
		myWindow->refresh();
	}
}

void ZLApplication::Action::checkAndRun() {
	if (isEnabled()) {
		run();
	}
}

shared_ptr<ZLApplication::Action> ZLApplication::action(int actionId) const {
	std::map<int,shared_ptr<Action> >::const_iterator it = myActionMap.find(actionId);
	return (it != myActionMap.end()) ? it->second : 0;
}

bool ZLApplication::isActionVisible(int actionId) const {
	shared_ptr<Action> a = action(actionId);
	return !a.isNull() && a->isVisible();
}

bool ZLApplication::isActionEnabled(int actionId) const {
	shared_ptr<Action> _action = action(actionId);
	return !_action.isNull() && _action->isEnabled();
}

void ZLApplication::doAction(int actionId) {
	shared_ptr<Action> _action = action(actionId);
	if (!_action.isNull()) {
		_action->checkAndRun();
	}
}

void ZLApplication::resetWindowCaption() {
	if (myWindow != 0) {
		if ((currentView() == 0) || (currentView()->caption().empty())) {
			myWindow->setCaption(ApplicationName());
		} else {
			myWindow->setCaption(ApplicationName() + " - " + currentView()->caption());
		}
	}
}

ZLApplication::Toolbar::Toolbar() : myResource(ZLResource::resource("toolbar")) {
}

void ZLApplication::Toolbar::addButton(int actionId, const ZLResourceKey &key, shared_ptr<ButtonGroup> group) {
	ButtonItem *button = new ButtonItem(actionId, key.Name, myResource[key]);
	myItems.push_back(button);
	button->setButtonGroup(group);
}

void ZLApplication::Toolbar::addOptionEntry(shared_ptr<ZLOptionEntry> entry) {
	if (!entry.isNull()) {
		myItems.push_back(new OptionEntryItem(entry));
	}
}

void ZLApplication::Toolbar::addSeparator() {
	myItems.push_back(new SeparatorItem());
}

ZLApplication::Action::~Action() {
}

bool ZLApplication::Action::isVisible() {
	return true;
}

bool ZLApplication::Action::isEnabled() {
	return isVisible();
}

bool ZLApplication::Action::useKeyDelay() const {
	return true;
}

void ZLApplication::trackStylus(bool track) {
	if (myViewWidget != 0) {
		myViewWidget->trackStylus(track);
	}
}

void ZLApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	bool enableToolbarSpace = false;
	ZLApplication::Toolbar::ItemPtr lastSeparator = 0;
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		switch ((*it)->type()) {
			case ZLApplication::Toolbar::Item::OPTION_ENTRY:
				if (((const ZLApplication::Toolbar::OptionEntryItem&)**it).entry()->isVisible()) {
					if (!lastSeparator.isNull()) {
						setToolbarItemState(lastSeparator, true, true);
						lastSeparator = 0;
					}
					enableToolbarSpace = true;
				}
				break;
			case ZLApplication::Toolbar::Item::BUTTON:
				{
					const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
					int id = button.actionId();
        
					const bool visible = application().isActionVisible(id);
					const bool enabled = application().isActionEnabled(id);
        
					if (visible) {
						if (!lastSeparator.isNull()) {
							setToolbarItemState(lastSeparator, true, true);
							lastSeparator = 0;
						}
						enableToolbarSpace = true;
					}
					if (!enabled && button.isPressed()) {
						shared_ptr<ZLApplication::Toolbar::ButtonGroup> group = button.buttonGroup();
						group->press(0);
						application().doAction(group->UnselectAllButtonsActionId);
						myToggleButtonLock = true;
						setToggleButtonState(button);
						myToggleButtonLock = false;
					}
					setToolbarItemState(*it, visible, enabled);
				}
				break;
			case ZLApplication::Toolbar::Item::SEPARATOR:
				if (enableToolbarSpace) {
					lastSeparator = *it;
					enableToolbarSpace = false;
				} else {
					setToolbarItemState(*it, false, true);
				}
				break;
		}
	}
	if (!lastSeparator.isNull()) {
		setToolbarItemState(lastSeparator, false, true);
	}
}

ZLApplication::Toolbar::ButtonGroup::ButtonGroup(int unselectAllButtonsActionId) : UnselectAllButtonsActionId(unselectAllButtonsActionId), PressedItem(0) {
}

void ZLApplication::Toolbar::ButtonGroup::press(const ButtonItem *item) {
	PressedItem = item;
}

void ZLApplication::Toolbar::ButtonItem::setButtonGroup(shared_ptr<ButtonGroup> group) {
	if (!myButtonGroup.isNull()) {
		myButtonGroup->Items.erase(this);
	}
	myButtonGroup = group;
	if (!myButtonGroup.isNull()) {
		myButtonGroup->Items.insert(this);
	}
}

void ZLApplication::doActionByKey(const std::string &key) {
	shared_ptr<Action> a = action(keyBindings().getBinding(key));
	if (!a.isNull() &&
			(!a->useKeyDelay() ||
			 (myLastKeyActionTime.millisecondsTo(ZLTime()) >= KeyDelayOption.value()))) {
		a->checkAndRun();
		myLastKeyActionTime = ZLTime();
	}
}

const std::string &ZLApplication::Toolbar::ButtonItem::tooltip() const {
	if (!myTooltip.hasValue()) {
		static const std::string EMPTY;
		return EMPTY;
	}
	return myTooltip.value();
}

bool ZLApplication::isFullKeyboardControlSupported() const {
	return (myWindow != 0) && myWindow->isFullKeyboardControlSupported();
}

bool ZLApplication::isFingerTapEventSupported() const {
	return (myWindow != 0) && myWindow->isFingerTapEventSupported();
}

bool ZLApplication::isMousePresented() const {
	return (myWindow != 0) && myWindow->isMousePresented();
}

bool ZLApplication::isKeyboardPresented() const {
	return (myWindow != 0) && myWindow->isKeyboardPresented();
}

void ZLApplication::grabAllKeys(bool grab) {
	if (myWindow != 0) {
		myWindow->grabAllKeys(grab);
	}
}

void ZLApplication::setHyperlinkCursor(bool hyperlink) {
	if (myWindow != 0) {
		myWindow->setHyperlinkCursor(hyperlink);
	}
}

bool ZLApplication::isFullscreen() const {
	return (myWindow != 0) && myWindow->isFullscreen();
}

void ZLApplication::setFullscreen(bool fullscreen) {
	if (myWindow != 0) {
		myWindow->setFullscreen(fullscreen);
	}
}

void ZLApplication::quit() {
	if (myWindow != 0) {
		myWindow->close();
	}
}

ZLApplication::Toolbar::Item::Type ZLApplication::Toolbar::ButtonItem::type() const {
	return BUTTON;
}

ZLApplication::Toolbar::Item::Type ZLApplication::Toolbar::SeparatorItem::type() const {
	return SEPARATOR;
}

ZLApplication::Toolbar::OptionEntryItem::OptionEntryItem(shared_ptr<ZLOptionEntry> entry) : myOptionEntry(entry) {
}

shared_ptr<ZLOptionEntry> ZLApplication::Toolbar::OptionEntryItem::entry() const {
	return myOptionEntry;
}

ZLApplication::Toolbar::Item::Type ZLApplication::Toolbar::OptionEntryItem::type() const {
	return OPTION_ENTRY;
}
