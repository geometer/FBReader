/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLApplication.h"
#include "../library/ZLibrary.h"
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
	ConfigAutoSaveTimeoutOption(ZLOption::CONFIG_CATEGORY, CONFIG, TIMEOUT, 1, 6000, 10),
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
	resetWindowCaption();
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

void ZLApplication::setView(ZLView *view) {
	if (myViewWidget != 0) {
		myViewWidget->setView(view);
		resetWindowCaption();
		refreshWindow();
	}
}

ZLView *ZLApplication::currentView() {
	return (myViewWidget != 0) ? myViewWidget->view() : 0;
}

void ZLApplication::refreshWindow() {
	if (myWindow != 0) {
		myWindow->refresh();
	}
	if (myViewWidget != 0) {
		myViewWidget->repaint();
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
	shared_ptr<Action> _action = action(actionId);
	return !_action.isNull() && _action->isVisible();
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

void ZLApplication::Toolbar::addButton(int actionId, const std::string &iconName, shared_ptr<ButtonGroup> group) {
	ButtonItem *button = new ButtonItem(actionId, iconName);
	myItems.push_back(button);
	button->setButtonGroup(group);
}

void ZLApplication::Toolbar::addSeparator() {
	myItems.push_back(new SeparatorItem());
}

void ZLApplication::Menu::addItem(const std::string &itemName, int actionId) {
	myItems.push_back(new Menubar::PlainItem(itemName, actionId));
}

void ZLApplication::Menu::addSeparator() {
	myItems.push_back(new Menubar::Separator());
}

ZLApplication::Menu::Menu &ZLApplication::Menu::addSubmenu(const std::string &menuName) {
	Menubar::Submenu *submenu = new Menubar::Submenu(menuName);
	myItems.push_back(submenu);
	return *submenu;
}

ZLApplication::Action::~Action() {
}

bool ZLApplication::Action::isVisible() {
	return true;
}

bool ZLApplication::Action::isEnabled() {
	return isVisible();
}

void ZLApplication::MenuVisitor::processMenu(ZLApplication::Menu &menu) {
	const ZLApplication::Menu::ItemVector &items = menu.items();
	for (ZLApplication::Menu::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		switch ((*it)->type()) {
			case ZLApplication::Menu::Item::ITEM:
				processItem((ZLApplication::Menubar::PlainItem&)**it);
				break;
			case ZLApplication::Menu::Item::SUBMENU:
			{
				ZLApplication::Menubar::Submenu &submenu = (ZLApplication::Menubar::Submenu&)**it;
				processSubmenuBeforeItems(submenu);
				processMenu(submenu);
				processSubmenuAfterItems(submenu);
				break;
			}
			case ZLApplication::Menu::Item::SEPARATOR:
				processSepartor((ZLApplication::Menubar::Separator&)**it);
				break;
		}							
	}
}

void ZLApplication::trackStylus(bool track) {
	if (myViewWidget != 0) {
		myViewWidget->trackStylus(track);
	}
}

void ZLApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	bool enableToolbarSpace = false;
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			int id = button.actionId();
    
			const bool visible = application().isActionVisible(id);
			const bool enabled = application().isActionEnabled(id);
    
			if (visible) {
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
		} else {
			setToolbarItemState(*it, enableToolbarSpace, true);
			enableToolbarSpace = false;
		}
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
