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

#ifndef __ZLAPPLICATION_H__
#define __ZLAPPLICATION_H__

#include <string>
#include <vector>
#include <map>
#include <set>

#include <shared_ptr.h>
#include <ZLOptions.h>
#include <ZLTime.h>
#include <ZLResource.h>

class ZLView;
class ZLViewWidget;
class ZLPaintContext;
class ZLKeyBindings;

class ZLApplicationBase {

public:
	static const std::string BaseDirectory;
	static const std::string HomeDirectory;

	static const std::string FileNameDelimiter;
	static const std::string PathDelimiter;

	static const std::string EndOfLine;

	static const std::string &ApplicationName();
	static const std::string &ImageDirectory();
	static const std::string &ApplicationImageDirectory();

	static const std::string &ZLibraryDirectory();
	static const std::string &ApplicationDirectory();
	static const std::string &DefaultFilesPathPrefix();

private:
	static std::string ourImageDirectory;
	static std::string ourApplicationImageDirectory;
	static std::string ourApplicationName;
	static std::string ourZLibraryDirectory;
	static std::string ourApplicationDirectory;
	static std::string ourDefaultFilesPathPrefix;

protected:
	ZLApplicationBase(const std::string &name);
	~ZLApplicationBase();

private:
	std::string replaceRegExps(const std::string &pattern);
};

class ZLApplication : public ZLApplicationBase {

public:
	static const std::string MouseScrollUpKey;
	static const std::string MouseScrollDownKey;

public:
	ZLIntegerOption RotationAngleOption;
	ZLIntegerOption AngleStateOption;

	ZLBooleanOption KeyboardControlOption;

	ZLBooleanOption ConfigAutoSavingOption;
	ZLIntegerRangeOption ConfigAutoSaveTimeoutOption;

	ZLIntegerRangeOption KeyDelayOption;

public:

	class Action {

	public:
		virtual ~Action();
		virtual bool isVisible();
		virtual bool isEnabled();
		void checkAndRun();
		virtual bool useKeyDelay() const;

	protected:
		virtual void run() = 0;
	};

protected:
	class RotationAction : public Action {

	public:
		RotationAction(ZLApplication &application);
		bool isVisible();
		void run();

	private:
		ZLApplication &myApplication;
	};
	friend class RotationAction;
	
	class FullscreenAction : public Action {

	public:
		FullscreenAction(ZLApplication &application, bool toggle);
		bool isVisible();
		void run();

	private:
		ZLApplication &myApplication;
		bool myIsToggle;
	};
	friend class FullscreenAction;
	
public:

	class Toolbar {

	public:
		class Item {

		public:
			Item();
			virtual ~Item();

			virtual bool isButton() const = 0;
			bool isSeparator() const;

		friend class Toolbar;
		};
	
		class ButtonGroup;

		class ButtonItem : public Item {

		public:
			ButtonItem(int actionId, const std::string &iconName, const ZLResource &tooltip);

			bool isButton() const;

			int actionId() const;
			const std::string &iconName() const;
			const std::string &tooltip() const;

			shared_ptr<ButtonGroup> buttonGroup() const;
			bool isToggleButton() const;
			void press();
			bool isPressed() const;

		private:
			void setButtonGroup(shared_ptr<ButtonGroup>);

		private:
			const int myActionId;
			const std::string myIconName;
			const ZLResource &myTooltip;
			shared_ptr<ButtonGroup> myButtonGroup;

		friend class Toolbar;
		};
	
		struct ButtonGroup {

		private:
			ButtonGroup(int unselectAllButtonsActionId);
			void press(const ButtonItem *item);

		private:
			const int UnselectAllButtonsActionId;
			std::set<const ButtonItem*> Items;
			const ButtonItem *PressedItem;

		friend class ButtonItem;
		friend class Toolbar;
		friend class ZLApplicationWindow;
		};

		class SeparatorItem : public Item {

		public:
			bool isButton() const;
		};

	public:
		typedef shared_ptr<Item> ItemPtr;
		typedef std::vector<ItemPtr> ItemVector;

		Toolbar();
		void addButton(int actionId, const ZLResourceKey &key, shared_ptr<ButtonGroup> group = 0);
		shared_ptr<ButtonGroup> createButtonGroup(int unselectAllButtonsActionId);
		void addSeparator();

		const ItemVector &items() const;

	private:
		ItemVector myItems;
		const ZLResource &myResource;

	friend class ZLApplication;
	};

	class Menu {

	public:
		class Item {

		public:
			enum ItemType {
				ITEM,
				SUBMENU,
				SEPARATOR
			};
			
		public:
			virtual ~Item();
			ItemType type() const;

		protected:
			Item(ItemType type);

		private:
			const ItemType myType;
		};

		typedef shared_ptr<Item> ItemPtr;
		typedef std::vector<ItemPtr> ItemVector;

		void addItem(const std::string &itemName, int actionId);
		void addSeparator();
		Menu& addSubmenu(const std::string &menuName);

		const ItemVector &items() const;

		virtual ~Menu();

	protected:
		Menu();

	private:
		ItemVector myItems;

	friend class ZLApplication;
	};

	class Menubar : public Menu {

	public:
		class PlainItem : public Menu::Item {

		public:
			PlainItem(const std::string &name, int actionId);

			const std::string &name() const;
			int actionId() const;

		private:
			const std::string myName;
			int myActionId;
		};

		class Submenu : public Menu::Item, public Menu {

		public:
			Submenu(const std::string &menuName);

			const std::string &menuName() const;

		private:
			const std::string myMenuName;
		};

		class Separator : public Menu::Item {

		public:
			Separator();
		};

	public:
		Menubar();

	friend class ZLApplication;
	};

	class MenuVisitor {

	public:
		virtual ~MenuVisitor();
		void processMenu(ZLApplication::Menu &menu);

	protected:
		virtual void processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu) = 0;
		virtual void processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu) = 0;
		virtual void processItem(ZLApplication::Menubar::PlainItem &item) = 0;
		virtual void processSepartor(ZLApplication::Menubar::Separator &separator) = 0;
	};

protected:
	ZLApplication(const std::string &name);

	void addAction(int actionId, shared_ptr<Action> action);

	void setView(shared_ptr<ZLView> view);
	shared_ptr<ZLView> currentView() const;

	void setFullscreen(bool fullscreen);
	bool isFullscreen() const;
	void resetWindowCaption();
	void quit();

public:
	virtual ~ZLApplication();
	virtual void initWindow();

	ZLPaintContext &context();

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

	bool isFingerTapEventSupported() const;
	bool isMousePresented() const;
	bool isKeyboardPresented() const;
	void trackStylus(bool track);

	void setHyperlinkCursor(bool hyperlink);

	shared_ptr<Action> action(int actionId) const;
	bool isActionVisible(int actionId) const;
	bool isActionEnabled(int actionId) const;
	void doAction(int actionId);

	virtual ZLKeyBindings &keyBindings() = 0;	
	void doActionByKey(const std::string &key);

	virtual bool closeView();
	virtual void openFile(const std::string &fileName);

	Toolbar &toolbar();
	const Toolbar &toolbar() const;

	Menubar &menubar();
	const Menubar &menubar() const;

	void refreshWindow();

protected:
	ZLViewWidget *myViewWidget;

private:
	shared_ptr<ZLView> myInitialView;
	std::map<int,shared_ptr<Action> > myActionMap;
	Toolbar myToolbar;
	Menubar myMenubar;
	ZLPaintContext *myContext;
	class ZLApplicationWindow *myWindow;
	ZLTime myLastKeyActionTime;

friend class ZLApplicationWindow;
};

class ZLApplicationWindow {

protected:
	ZLApplicationWindow(ZLApplication *application);

public:
	ZLApplication &application() const;

protected:
	virtual void init();
	// TODO: change to pure virtual
	virtual void initMenu() {}

	void onButtonPress(ZLApplication::Toolbar::ButtonItem &button);
	// TODO: change to pure virtual
	virtual void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem&) {}
	// TODO: change to pure virtual
	virtual void setToolbarItemState(ZLApplication::Toolbar::ItemPtr /*item*/, bool /*visible*/, bool /*enabled*/) {}

	virtual ZLViewWidget *createViewWidget() = 0;
	virtual void addToolbarItem(ZLApplication::Toolbar::ItemPtr item) = 0;

	// TODO: change to non-virtual (?)
	virtual void refresh();

	virtual void close() = 0;

	virtual void setCaption(const std::string &caption) = 0;

	virtual bool isFullKeyboardControlSupported() const = 0;
	virtual void grabAllKeys(bool grab) = 0;

	virtual bool isFingerTapEventSupported() const = 0;
	virtual bool isMousePresented() const = 0;
	virtual bool isKeyboardPresented() const = 0;

	virtual void setFullscreen(bool fullscreen) = 0;
	virtual bool isFullscreen() const = 0;

	// TODO: change to pure virtual (?)
	virtual void setHyperlinkCursor(bool) {}

public:
	virtual ~ZLApplicationWindow();

private:
	ZLApplication *myApplication;
	bool myToggleButtonLock;

friend class ZLApplication;
};


inline const std::string &ZLApplicationBase::ApplicationName() { return ourApplicationName; }
inline const std::string &ZLApplicationBase::ImageDirectory() { return ourImageDirectory; }
inline const std::string &ZLApplicationBase::ApplicationImageDirectory() { return ourApplicationImageDirectory; }
inline const std::string &ZLApplicationBase::ApplicationDirectory() { return ourApplicationDirectory; }
inline const std::string &ZLApplicationBase::ZLibraryDirectory() { return ourZLibraryDirectory; }
inline const std::string &ZLApplicationBase::DefaultFilesPathPrefix() { return ourDefaultFilesPathPrefix; }

inline ZLApplication::Toolbar &ZLApplication::toolbar() { return myToolbar; }
inline ZLApplication::Menubar &ZLApplication::menubar() { return myMenubar; }
inline const ZLApplication::Toolbar &ZLApplication::toolbar() const { return myToolbar; }
inline const ZLApplication::Menubar &ZLApplication::menubar() const { return myMenubar; }

inline ZLPaintContext &ZLApplication::context() {
	return *myContext;
}

inline bool ZLApplication::isFullKeyboardControlSupported() const {
	return (myWindow != 0) && myWindow->isFullKeyboardControlSupported();
}
inline bool ZLApplication::isFingerTapEventSupported() const {
	return (myWindow != 0) && myWindow->isFingerTapEventSupported();
}
inline bool ZLApplication::isMousePresented() const {
	return (myWindow != 0) && myWindow->isMousePresented();
}
inline bool ZLApplication::isKeyboardPresented() const {
	return (myWindow != 0) && myWindow->isKeyboardPresented();
}
inline void ZLApplication::grabAllKeys(bool grab) {
	if (myWindow != 0) {
		myWindow->grabAllKeys(grab);
	}
}
inline void ZLApplication::setHyperlinkCursor(bool hyperlink) {
	if (myWindow != 0) {
		myWindow->setHyperlinkCursor(hyperlink);
	}
}
inline bool ZLApplication::isFullscreen() const {
	return (myWindow != 0) && myWindow->isFullscreen();
}
inline void ZLApplication::setFullscreen(bool fullscreen) {
	if (myWindow != 0) {
		myWindow->setFullscreen(fullscreen);
	}
}
inline void ZLApplication::quit() {
	if (myWindow != 0) {
		myWindow->close();
	}
}
inline ZLApplicationWindow::~ZLApplicationWindow() {}
inline ZLApplication &ZLApplicationWindow::application() const { return *myApplication; }

inline const ZLApplication::Toolbar::ItemVector &ZLApplication::Toolbar::items() const { return myItems; }

inline ZLApplication::Toolbar::Item::Item() {}
inline ZLApplication::Toolbar::Item::~Item() {}
inline bool ZLApplication::Toolbar::Item::isSeparator() const { return !isButton(); }

inline ZLApplication::Toolbar::ButtonItem::ButtonItem(int actionId, const std::string &iconName, const ZLResource &tooltip) : myActionId(actionId), myIconName(iconName), myTooltip(tooltip) {}
inline bool ZLApplication::Toolbar::ButtonItem::isButton() const { return true; }
inline int ZLApplication::Toolbar::ButtonItem::actionId() const { return myActionId; }
inline const std::string &ZLApplication::Toolbar::ButtonItem::iconName() const { return myIconName; }
inline shared_ptr<ZLApplication::Toolbar::ButtonGroup> ZLApplication::Toolbar::ButtonItem::buttonGroup() const { return myButtonGroup; }
inline bool ZLApplication::Toolbar::ButtonItem::isToggleButton() const { return !myButtonGroup.isNull(); }
inline void ZLApplication::Toolbar::ButtonItem::press() { if (isToggleButton()) { myButtonGroup->press(this); } }
inline bool ZLApplication::Toolbar::ButtonItem::isPressed() const { return isToggleButton() && (this == myButtonGroup->PressedItem); }

inline bool ZLApplication::Toolbar::SeparatorItem::isButton() const { return false; }

inline shared_ptr<ZLApplication::Toolbar::ButtonGroup> ZLApplication::Toolbar::createButtonGroup(int unselectAllButtonsActionId) {
	return new ButtonGroup(unselectAllButtonsActionId);
}

inline ZLApplication::Menu::Menu() {}
inline ZLApplication::Menu::~Menu() {}
inline const ZLApplication::Menu::ItemVector &ZLApplication::Menu::items() const { return myItems; }

inline ZLApplication::Menu::Item::Item(ItemType type): myType(type) {}
inline ZLApplication::Menu::Item::~Item() {}
inline ZLApplication::Menu::Item::ItemType ZLApplication::Menubar::Item::type() const { return myType; }

inline ZLApplication::Menubar::Menubar() {}

inline ZLApplication::Menubar::PlainItem::PlainItem(const std::string& name, int actionId) : Item(ITEM), myName(name), myActionId(actionId) {}
inline const std::string& ZLApplication::Menubar::PlainItem::name() const { return myName; }
inline int ZLApplication::Menubar::PlainItem::actionId() const { return myActionId; }

inline ZLApplication::Menubar::Submenu::Submenu(const std::string &menuName) : Menu::Item(SUBMENU), myMenuName(menuName) {}
inline const std::string &ZLApplication::Menubar::Submenu::menuName() const { return myMenuName; }

inline ZLApplication::Menubar::Separator::Separator() : Item(SEPARATOR) {}

inline ZLApplication::MenuVisitor::~MenuVisitor() {}

#endif /* __ZLAPPLICATION_H__ */

// vim:noet:ts=2:sw=2
