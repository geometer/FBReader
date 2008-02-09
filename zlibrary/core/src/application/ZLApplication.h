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

#ifndef __ZLAPPLICATION_H__
#define __ZLAPPLICATION_H__

#include <string>
#include <vector>
#include <map>
#include <set>

#include <shared_ptr.h>
#include <ZLOptions.h>
#include <ZLOptionEntry.h>
#include <ZLTime.h>
#include <ZLResource.h>
#include <ZLMessage.h>

class ZLView;
class ZLViewWidget;
class ZLPaintContext;
class ZLKeyBindings;

class ZLApplicationBase {

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
	static const std::string NoAction;

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
			enum Type {
				BUTTON,
				OPTION_ENTRY,
				SEPARATOR
			};

		public:
			Item();
			virtual ~Item();

			virtual Type type() const = 0;

		friend class Toolbar;
		};
	
		class ButtonGroup;

		class ButtonItem : public Item {

		public:
			ButtonItem(const std::string &actionId, const ZLResource &tooltip);

			Type type() const;

			const std::string &actionId() const;
			const std::string &iconName() const;
			const std::string &tooltip() const;

			shared_ptr<ButtonGroup> buttonGroup() const;
			bool isToggleButton() const;
			void press();
			bool isPressed() const;

		private:
			void setButtonGroup(shared_ptr<ButtonGroup>);

		private:
			const std::string myActionId;
			const ZLResource &myTooltip;
			shared_ptr<ButtonGroup> myButtonGroup;

		friend class Toolbar;
		};
	
		struct ButtonGroup {

		private:
			ButtonGroup(const std::string &unselectAllButtonsActionId);
			void press(const ButtonItem *item);

		private:
			const std::string UnselectAllButtonsActionId;
			std::set<const ButtonItem*> Items;
			const ButtonItem *PressedItem;

		friend class ButtonItem;
		friend class Toolbar;
		friend class ZLApplicationWindow;
		};

		class SeparatorItem : public Item {

		public:
			Type type() const;
		};

		class OptionEntryItem : public Item {

		public:
			OptionEntryItem(shared_ptr<ZLOptionEntry> entry);
			Type type() const;
			shared_ptr<ZLOptionEntry> entry() const;

		private:
			shared_ptr<ZLOptionEntry> myOptionEntry;
		};

	public:
		typedef shared_ptr<Item> ItemPtr;
		typedef std::vector<ItemPtr> ItemVector;

		Toolbar();
		void addButton(const std::string &actionId, shared_ptr<ButtonGroup> group = 0);
		shared_ptr<ButtonGroup> createButtonGroup(const std::string &unselectAllButtonsActionId);
		void addOptionEntry(shared_ptr<ZLOptionEntry> entry);
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

		void addItem(const std::string &actionId, const ZLResourceKey &key);
		void addSeparator();
		ItemPtr addSubmenu(const ZLResourceKey &key);

		const ItemVector &items() const;

		virtual ~Menu();

	protected:
		Menu(const ZLResource &resource);

	protected:
		const ZLResource &myResource;

	private:
		ItemVector myItems;

	friend class ZLApplication;
	};

	class Menubar : public Menu {

	public:
		class PlainItem : public Menu::Item {

		public:
			PlainItem(const std::string &name, const std::string &actionId);

			const std::string &name() const;
			const std::string &actionId() const;

		private:
			const std::string myName;
			const std::string myActionId;
		};

		class Submenu : public Menu::Item, public Menu {

		public:
			Submenu(const ZLResource &resource);

			const std::string &menuName() const;
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
		void processMenu(ZLApplication &application);

	private:
		void processMenu(ZLApplication::Menu &menu);

	protected:
		virtual void processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu) = 0;
		virtual void processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu) = 0;
		virtual void processItem(ZLApplication::Menubar::PlainItem &item) = 0;
		virtual void processSepartor(ZLApplication::Menubar::Separator &separator) = 0;
	};

	class PresentWindowHandler : public ZLMessageHandler {

	public:
		PresentWindowHandler(ZLApplication &application);
		void onMessageReceived(const std::vector<std::string> &arguments);
		const std::string &lastCaller() const;
		void resetLastCaller();

	private:
		ZLApplication &myApplication;
		std::string myLastCaller;
	};

protected:
	ZLApplication(const std::string &name);

	void addAction(const std::string &actionId, shared_ptr<Action> action);

	void setView(shared_ptr<ZLView> view);
	shared_ptr<ZLView> currentView() const;

	void setFullscreen(bool fullscreen);
	bool isFullscreen() const;
	void resetWindowCaption();
	void quit();

public:
	virtual ~ZLApplication();
	virtual void initWindow();

	shared_ptr<ZLPaintContext> context();

	void grabAllKeys(bool grab);
	void trackStylus(bool track);

	void setHyperlinkCursor(bool hyperlink);

	shared_ptr<Action> action(const std::string &actionId) const;
	bool isActionVisible(const std::string &actionId) const;
	bool isActionEnabled(const std::string &actionId) const;
	void doAction(const std::string &actionId);

	virtual ZLKeyBindings &keyBindings() = 0;	
	void doActionByKey(const std::string &key);

	virtual bool closeView();
	virtual void openFile(const std::string &fileName);

	// TODO in 1.0: replace by virtual method
	bool isViewFinal() const;
	// TODO in 1.0: remove
	void setViewFinal(bool final);

	void refreshWindow();
	void presentWindow();

	const std::string &lastCaller() const;
	void resetLastCaller();

private:
	void createToolbar();
public:
	// TODO: change to private
	const Toolbar &toolbar() const;

private:
	void createMenubar();
	const Menubar &menubar() const;

protected:
	ZLViewWidget *myViewWidget;

private:
	shared_ptr<ZLView> myInitialView;
	std::map<std::string,shared_ptr<Action> > myActionMap;
	Toolbar myToolbar;
	Menubar myMenubar;
	shared_ptr<ZLPaintContext> myContext;
	class ZLApplicationWindow *myWindow;
	ZLTime myLastKeyActionTime;
	shared_ptr<ZLMessageHandler> myPresentWindowHandler;
	// TODO in 1.0: change to non-static
	static bool myIsViewFinal;

friend class ZLApplicationWindow;
friend class MenuVisitor;
};

#endif /* __ZLAPPLICATION_H__ */
