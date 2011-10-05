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

#ifndef __ZLQTAPPLICATIONWINDOW_H__
#define __ZLQTAPPLICATIONWINDOW_H__

#include <map>

#include <QtGui/QMainWindow>
#include <QtGui/QAction>
#include <QtGui/QCursor>
#include <QtDeclarative/qdeclarative.h>

#ifdef MEEGO_EDITION
#include <policy/resource-set.h>
#endif

class QDockWidget;
class QToolBar;
class QToolButton;
class QLineEdit;

class ZLPopupData;
class ZLQmlToolBarItem;
class ZLQmlMenuBar;

#include "../../../../core/src/application/ZLApplicationWindow.h"
#include "../../../../core/src/application/ZLMenu.h"

class ZLQmlApplicationWindow : public QObject, public ZLApplicationWindow {
	Q_OBJECT
	Q_PROPERTY(bool fullSreen READ isFullscreen NOTIFY fullScreenChanged)
	Q_PROPERTY(QDeclarativeListProperty<QObject> actions READ actions NOTIFY actionsChanged)
	Q_PROPERTY(QObject* menuBar READ menuBar CONSTANT)
	Q_PROPERTY(QString bookTitle READ bookTitle NOTIFY bookTitleChanged)

public:
	ZLQmlApplicationWindow(ZLApplication *application);
	~ZLQmlApplicationWindow();

	void setFocusToMainWidget();
	QObject *menuBar();
	
	using ZLApplicationWindow::onButtonPress;

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLToolbar::ItemPtr item);
	void init();
	void initMenu();
	void processAllEvents();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	void setHyperlinkCursor(bool hyperlink);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	QString bookTitle() const;

	void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);
	
	QDeclarativeListProperty<QObject> actions();

	bool eventFilter(QObject *, QEvent *);
	
Q_SIGNALS:
	void fullScreenChanged(bool fullScreen);
	void actionsChanged(const QDeclarativeListProperty<QObject> &actions);
	void bookTitleChanged();

private:
	QObjectList myActions;
	QHash<const ZLToolbar::Item*, ZLQmlToolBarItem*> myActionsHash;
	ZLQmlMenuBar *myMenu;
#ifdef MEEGO_EDITION
	ResourcePolicy::ResourceSet *myResourceSet;
#endif
	bool myFullScreen;
	QString myBookTitle;

private:
	class LineEditParameter : public VisualParameter {

	public:
		LineEditParameter(QToolBar *toolbar, ZLQmlApplicationWindow &window, const ZLToolbar::ParameterItem &textFieldItem);
		QAction *action() const;
		void restoreOldValue();

	private:
		std::string internalValue() const;
		void internalSetValue(const std::string &value);
		void setValueList(const std::vector<std::string> &values) {}

	private:
		QLineEdit *myEdit;
		QAction *myAction;
	};

friend class ZLQtLineEdit;
};

class ZLQmlMenuBar : public QObject {
	Q_OBJECT
	Q_PROPERTY(QStringList items READ items NOTIFY itemsChanged)
	Q_PROPERTY(QStringList visibleItems READ visibleItems NOTIFY visibleItemsChanged)
	Q_PROPERTY(QStringList enabledItems READ enabledItems NOTIFY enabledItemsChanged)
public:
	ZLQmlMenuBar(ZLQmlApplicationWindow *window);
	virtual ~ZLQmlMenuBar();
	
	void init();
	QStringList items();
	QStringList visibleItems();
	QStringList enabledItems();
	Q_INVOKABLE void activate(int index);
	Q_INVOKABLE void recheckItems();
	
public Q_SLOTS:
	void delayedActivate(int index);
	
Q_SIGNALS:
	void itemsChanged(const QStringList &items);
	void activated(int index);
	void visibleItemsChanged(const QStringList &items);
	void enabledItemsChanged(const QStringList &items);
	
private:
	QList<std::string> myIds;
	QStringList myNames;
	QStringList myVisibleItems;
	QStringList myEnabledItems;
	
	class Builder : public ZLMenuVisitor {
	public:
		Builder();
		virtual ~Builder();

		QStringList names();
		QList<std::string> ids();
		
	protected:
		virtual void processSubmenuBeforeItems(ZLMenubar::Submenu &submenu);
		virtual void processSubmenuAfterItems(ZLMenubar::Submenu &submenu);
		virtual void processItem(ZLMenubar::PlainItem &item);
		virtual void processSepartor(ZLMenubar::Separator &separator);
		
	private:
		QList<std::string> myIds;
		QStringList myNames;
	};
};

class ZLQmlToolBarItem : public QObject
{
	Q_OBJECT
	Q_ENUMS(Type)
	Q_PROPERTY(Type type READ type CONSTANT)
	Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
	Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)
public:
	enum Type {
		PlainButton = ZLToolbar::Item::PLAIN_BUTTON,
		MenuButton = ZLToolbar::Item::MENU_BUTTON,
		ToggleButton = ZLToolbar::Item::TOGGLE_BUTTON,
		TextField = ZLToolbar::Item::TEXT_FIELD,
		ComboBox = ZLToolbar::Item::COMBO_BOX,
		SearchField = ZLToolbar::Item::SEARCH_FIELD,
		Separator = ZLToolbar::Item::SEPARATOR,
		FillSeparator = ZLToolbar::Item::FILL_SEPARATOR
	};
	
	ZLQmlToolBarItem(ZLToolbar::Item::Type type, QObject *parent);
	virtual ~ZLQmlToolBarItem();
	
	Type type() const;
	bool enabled() const;
	void setEnabled(bool enabled);
	bool visible() const;
	void setVisible(bool visible);
	
Q_SIGNALS:
	void visibleChanged(bool visible);
	void enabledChanged(bool enabled);
	
private:
	bool myVisible;
	bool myEnabled;
	Type myType;
};

class ZLQmlToolBarAction : public ZLQmlToolBarItem
{
	Q_OBJECT
	Q_PROPERTY(QString iconSource READ iconSource CONSTANT)
	Q_PROPERTY(QString platformIconId READ platformIconId CONSTANT)
	Q_PROPERTY(bool checked READ checked NOTIFY checkedChanged)
public:
	ZLQmlToolBarAction(ZLToolbar::AbstractButtonItem &item, QObject *parent);
	virtual ~ZLQmlToolBarAction();
	
	QString iconSource() const;
	QString platformIconId() const;
	bool checked() const;
	void setChecked(bool checked);
	Q_INVOKABLE void activate();
	
Q_SIGNALS:
	void checkedChanged(bool checked);
	
private Q_SLOTS:
	void delayedActivate();
private:
	QString myIconSource;
	QString myPlatformIconId;
	bool myChecked;
	ZLToolbar::AbstractButtonItem &myItem;
};

class ZLQmlToolBarMenu : public ZLQmlToolBarAction
{
	Q_OBJECT
	Q_PROPERTY(QStringList items READ items NOTIFY itemsChanged)
public:
	ZLQmlToolBarMenu(ZLToolbar::MenuButtonItem &item, QObject *parent);
	virtual ~ZLQmlToolBarMenu();
	
	QStringList items();
	void setPopupData(const shared_ptr<ZLPopupData> &popupData);
	Q_INVOKABLE void activate(int index);
	
Q_SIGNALS:
	void itemsChanged(const QStringList &items);

private:
	QStringList myItems;
	shared_ptr<ZLPopupData> myPopupData;
};

#endif /* __ZLQTAPPLICATIONWINDOW_H__ */
