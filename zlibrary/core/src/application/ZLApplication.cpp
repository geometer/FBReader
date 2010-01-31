/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include "ZLToolbar.h"
#include "ZLMenu.h"
#include "ZLPopupData.h"
#include "../view/ZLView.h"
#include "../view/ZLViewWidget.h"
#include "../view/ZLPaintContext.h"

const std::string ZLApplication::MouseScrollDownKey = "<MouseScrollDown>";
const std::string ZLApplication::MouseScrollUpKey = "<MouseScrollUp>";
const std::string ZLApplication::NoAction = "none";

static const std::string ROTATION = "Rotation";
static const std::string ANGLE = "Angle";
static const std::string STATE = "State";
static const std::string KEYBOARD = "Keyboard";
static const std::string FULL_CONTROL = "FullControl";
static const std::string CONFIG = "Config";
static const std::string AUTO_SAVE = "AutoSave";
static const std::string TIMEOUT = "Timeout";

ZLApplication *ZLApplication::ourInstance = 0;

ZLApplication &ZLApplication::Instance() {
	return *ourInstance;
}

void ZLApplication::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
		ourInstance = 0;
	}
}

ZLApplication::ZLApplication(const std::string &name) : ZLApplicationBase(name),
	RotationAngleOption(ZLCategoryKey::CONFIG, ROTATION, ANGLE, ZLView::DEGREES90),
	AngleStateOption(ZLCategoryKey::CONFIG, STATE, ANGLE, ZLView::DEGREES0),
	KeyboardControlOption(ZLCategoryKey::CONFIG, KEYBOARD, FULL_CONTROL, false),
	ConfigAutoSavingOption(ZLCategoryKey::CONFIG, CONFIG, AUTO_SAVE, true),
	ConfigAutoSaveTimeoutOption(ZLCategoryKey::CONFIG, CONFIG, TIMEOUT, 1, 6000, 30),
	KeyDelayOption(ZLCategoryKey::CONFIG, "Options", "KeyDelay", 0, 5000, 250) {
	ourInstance = this;
	myContext = ZLibrary::createContext();
	if (ConfigAutoSavingOption.value()) {
		ZLOption::startAutoSave(ConfigAutoSaveTimeoutOption.value());
	}

	myPresentWindowHandler = new PresentWindowHandler();
	ZLCommunicationManager::Instance().registerHandler("present", myPresentWindowHandler);

	createToolbar(ZLApplicationWindow::WINDOW_TOOLBAR);
	createToolbar(ZLApplicationWindow::FULLSCREEN_TOOLBAR);
	createMenubar();
}

ZLApplication::~ZLApplication() {
	if (!myWindow.isNull()) {
		if (KeyboardControlOption.value()) {
			grabAllKeys(false);
		}
	}
	ourInstance = 0;
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

bool ZLApplication::canDragFiles(const std::vector<std::string>&) const {
	return false;
}

void ZLApplication::dragFiles(const std::vector<std::string>&) {
}

void ZLApplication::addAction(const std::string &actionId, shared_ptr<Action> action) {
	myActionMap[actionId] = action;
}

void ZLApplication::setView(shared_ptr<ZLView> view) {
	if (view.isNull()) {
		return;
	}

	if (!myViewWidget.isNull()) {
		myViewWidget->setView(view);
		resetWindowCaption();
		refreshWindow();
	} else {
		myInitialView = view;
	}
}

shared_ptr<ZLView> ZLApplication::currentView() const {
	return myViewWidget.isNull() ? 0 : myViewWidget->view();
}

void ZLApplication::refreshWindow() {
	if (!myViewWidget.isNull()) {
		myViewWidget->repaint();
	}
	if (!myWindow.isNull()) {
		myWindow->refresh();
	}
}

void ZLApplication::presentWindow() {
	if (!myWindow.isNull()) {
		myWindow->present();
	}
}

void ZLApplication::Action::checkAndRun() {
	if (isEnabled()) {
		run();
	}
}

shared_ptr<ZLApplication::Action> ZLApplication::action(const std::string &actionId) const {
	std::map<std::string,shared_ptr<Action> >::const_iterator it = myActionMap.find(actionId);
	return (it != myActionMap.end()) ? it->second : 0;
}

bool ZLApplication::isActionVisible(const std::string &actionId) const {
	shared_ptr<Action> a = action(actionId);
	return !a.isNull() && a->isVisible();
}

bool ZLApplication::isActionEnabled(const std::string &actionId) const {
	shared_ptr<Action> _action = action(actionId);
	return !_action.isNull() && _action->isEnabled();
}

void ZLApplication::doAction(const std::string &actionId) {
	shared_ptr<Action> _action = action(actionId);
	if (!_action.isNull()) {
		_action->checkAndRun();
	}
}

void ZLApplication::resetWindowCaption() {
	if (!myWindow.isNull()) {
		if ((currentView() == 0) || (currentView()->caption().empty())) {
			myWindow->setCaption(ZLibrary::ApplicationName());
		} else {
			myWindow->setCaption(ZLibrary::ApplicationName() + " - " + currentView()->caption());
		}
	}
}

ZLApplication::Action::~Action() {
}

bool ZLApplication::Action::isVisible() const {
	return true;
}

bool ZLApplication::Action::isEnabled() const {
	return isVisible();
}

bool ZLApplication::Action::useKeyDelay() const {
	return true;
}

void ZLApplication::trackStylus(bool track) {
	if (!myViewWidget.isNull()) {
		myViewWidget->trackStylus(track);
	}
}

void ZLApplication::doActionByKey(const std::string &key) {
	shared_ptr<ZLKeyBindings> bindings = keyBindings();
	if (bindings.isNull()) {
		return;
	}
	shared_ptr<Action> a = action(bindings->getBinding(key));
	if (!a.isNull() &&
			(!a->useKeyDelay() ||
			 (myLastKeyActionTime.millisecondsTo(ZLTime()) >= KeyDelayOption.value()))) {
		a->checkAndRun();
		myLastKeyActionTime = ZLTime();
	}
}

void ZLApplication::grabAllKeys(bool grab) {
	if (!myWindow.isNull()) {
		myWindow->grabAllKeys(grab);
	}
}

void ZLApplication::setHyperlinkCursor(bool hyperlink) {
	if (!myWindow.isNull()) {
		myWindow->setHyperlinkCursor(hyperlink);
	}
}

bool ZLApplication::isFullscreen() const {
	return !myWindow.isNull() && myWindow->isFullscreen();
}

void ZLApplication::setFullscreen(bool fullscreen) {
	if (!myWindow.isNull()) {
		myWindow->setFullscreen(fullscreen);
	}
}

void ZLApplication::quit() {
	if (!myWindow.isNull()) {
		myWindow->close();
	}
}

void ZLApplication::PresentWindowHandler::onMessageReceived(const std::vector<std::string> &arguments) {
	ZLApplication::Instance().presentWindow();
	if (arguments.size() == 1) {
		myLastCaller = arguments[0];
	}
}

const std::string &ZLApplication::PresentWindowHandler::lastCaller() const {
	return myLastCaller;
}

void ZLApplication::PresentWindowHandler::resetLastCaller() {
	myLastCaller.erase();
}

const std::string &ZLApplication::lastCaller() const {
	return ((PresentWindowHandler&)*myPresentWindowHandler).lastCaller();
}

void ZLApplication::resetLastCaller() {
	((PresentWindowHandler&)*myPresentWindowHandler).resetLastCaller();
}

shared_ptr<ZLPaintContext> ZLApplication::context() {
	return myContext;
}

bool ZLApplication::isViewFinal() const {
	return true;
}

void ZLApplication::setVisualParameter(const std::string &id, const std::string &value) {
	if (!myWindow.isNull()) {
		myWindow->setVisualParameter(id, value);
	}
}

void ZLApplication::setParameterValueList(const std::string &id, const std::vector<std::string> &values) {
	if (!myWindow.isNull()) {
		myWindow->setParameterValueList(id, values);
	}
}

const std::string &ZLApplication::visualParameter(const std::string &id) {
	if (!myWindow.isNull()) {
		return myWindow->visualParameter(id);
	}
	static const std::string EMPTY;
	return EMPTY;
}

ZLView::Angle ZLApplication::rotation() const {
	return myViewWidget.isNull() ? ZLView::DEGREES0 : myViewWidget->rotation();
}

shared_ptr<ZLKeyBindings> ZLApplication::keyBindings() {
	return 0;
}
