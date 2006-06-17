/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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
#include <abstract/shared_ptr.h>

class ZLApplication {

public:
	class Toolbar {

	public:
		class Item {

		public:
			Item();
			virtual ~Item();

			bool isVisible() const;

			virtual bool isButton() const = 0;
			bool isSeparator() const;

		private:
			void setVisible(bool visible);

		private:
			bool myIsVisible;

		friend class Toolbar;
		};
	
		class ButtonItem : public Item {

		public:
			ButtonItem(int actionId, const std::string &iconName);

			bool isButton() const;

			bool isEnabled() const;

			int actionId() const;
			const std::string &iconName() const;

		private:
			void setEnabled(bool enabled);

		private:
			bool myIsEnabled;
			const int myActionId;
			const std::string myIconName;

		friend class Toolbar;
		};
	
		class SeparatorItem : public Item {

		public:
			bool isButton() const;
		};

	public:
		typedef shared_ptr<Item> ItemPtr;
		typedef std::vector<ItemPtr> ItemVector;

		Toolbar();

		bool isVisibilityChanged() const;
		void reset();

		void addButton(int actionId, const std::string &iconName);
		void addSeparator();

		const ItemVector &items() const;

	private:
		void setActionVisible(int actionId, bool visible);
		void setActionEnabled(int actionId, bool enabled);

	private:
		ItemVector myItems;
		std::map<int, ItemPtr> myItemsById;
		bool myVisibilityChanged;

	friend class ZLApplication;
	};

protected:
	ZLApplication();

public:
	virtual ~ZLApplication();

	void initView(class ZLApplicationView *view);
	void refreshView();

  virtual void setWindowCaption(const std::string &caption) = 0;

	Toolbar &toolbar();

	void setActionVisible(int actionId, bool visible);
	void setActionEnabled(int actionId, bool enabled);

	virtual void enableMenuButtons() = 0;

private:
	Toolbar myToolbar;
	class ZLApplicationView *myView;
};

class ZLApplicationView {

protected:
	ZLApplicationView();

  void init();
	virtual void refresh() = 0;
	virtual void addToolbarItem(ZLApplication::Toolbar::ItemPtr item) = 0;

public:
	virtual ~ZLApplicationView();

private:
	ZLApplication *myApplication;

friend void ZLApplication::initView(ZLApplicationView *view);
friend void ZLApplication::refreshView();
};

inline ZLApplication::ZLApplication() : myView(0) {}
inline ZLApplication::~ZLApplication() {
	if (myView != 0) {
		//delete myView;
	}
}
inline ZLApplication::Toolbar &ZLApplication::toolbar() { return myToolbar; }
inline void ZLApplication::initView(ZLApplicationView *view) {
	myView = view;
	myView->myApplication = this;
	myView->init();
	myView->refresh();
}
inline void ZLApplication::refreshView() {
	if (myView != 0) {
		myView->refresh();
	}
}

inline ZLApplicationView::ZLApplicationView() {}
inline ZLApplicationView::~ZLApplicationView() {}

inline const ZLApplication::Toolbar::ItemVector &ZLApplication::Toolbar::items() const { return myItems; }
inline bool ZLApplication::Toolbar::isVisibilityChanged() const { return myVisibilityChanged; }

inline ZLApplication::Toolbar::Item::Item() : myIsVisible(true) {}
inline ZLApplication::Toolbar::Item::~Item() {}
inline bool ZLApplication::Toolbar::Item::isVisible() const { return myIsVisible; }
inline void ZLApplication::Toolbar::Item::setVisible(bool visible) { myIsVisible = visible; }
inline bool ZLApplication::Toolbar::Item::isSeparator() const { return !isButton(); }

inline ZLApplication::Toolbar::ButtonItem::ButtonItem(int actionId, const std::string &iconName) : myIsEnabled(true), myActionId(actionId), myIconName(iconName) {}
inline bool ZLApplication::Toolbar::ButtonItem::isEnabled() const { return myIsEnabled; }
inline void ZLApplication::Toolbar::ButtonItem::setEnabled(bool enabled) { myIsEnabled = enabled; }
inline bool ZLApplication::Toolbar::ButtonItem::isButton() const { return true; }
inline int ZLApplication::Toolbar::ButtonItem::actionId() const { return myActionId; }
inline const std::string &ZLApplication::Toolbar::ButtonItem::iconName() const { return myIconName; }

inline bool ZLApplication::Toolbar::SeparatorItem::isButton() const { return false; }

#endif /* __ZLAPPLICATION_H__ */
