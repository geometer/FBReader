/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTDIALOGMANAGER_H__
#define __ZLQTDIALOGMANAGER_H__

#include <ZLDialogManager.h>
#include <QtCore/QObject>
#include <QtCore/QWeakPointer>
#include <QtCore/QEvent>

class QWidget;

class ZLQmlDialogManager : public QObject, public ZLDialogManager {
	Q_OBJECT

public:
	static void createInstance() { ourInstance = new ZLQmlDialogManager(); }

private:
	ZLQmlDialogManager();

public:
	void createApplicationWindow(ZLApplication *application) const;
	
	Q_INVOKABLE void kill(QObject *dialog);

	shared_ptr<ZLDialog> createDialog(const ZLResourceKey &key) const;
	shared_ptr<ZLOptionsDialog> createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const;
	shared_ptr<ZLOpenFileDialog> createOpenFileDialog(const ZLResourceKey &key, const std::string &directoryPath, const std::string &filePath, const ZLOpenFileDialog::Filter &filter) const;
	shared_ptr<ZLTreeDialog> createTreeDialog() const;
	shared_ptr<ZLDialogContent> createContent(const ZLResourceKey &key) const;
	void informationBox(const std::string &title, const std::string &message) const;
	void errorBox(const ZLResourceKey &key, const std::string &message) const;
	int questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const;
	shared_ptr<ZLProgressDialog> createProgressDialog(const ZLResourceKey &key) const;

	bool openURL(const std::string &url) const;
	bool isClipboardSupported(ClipboardType type) const;
	void setClipboardText(const std::string &text, ClipboardType type) const;
	void setClipboardImage(const ZLImageData &imageData, ClipboardType type) const;
	
private Q_SLOTS:
	void onObjectDestroyed(QObject *object);

Q_SIGNALS:
	void dialogRequested(QObject *object);
	void fileDialogRequested(QObject *object);
	void optionsDialogRequested(QObject *object);
	void treeDialogRequested(QObject *object);
	void progressDialogRequested(QObject *object);
	void questionDialogRequested(QObject *object);
	
	void privateInformationBoxRequested(const QString &title, const QString &message, const QString &button);
	void informationBoxRequested(const QString &title, const QString &message, const QString &button);
	void privateErrorBoxRequested(const QString &title, const QString &message, const QString &button);
	void errorBoxRequested(const QString &title, const QString &message, const QString &button);
//	void privateQuestionBoxRequested(QObject *object);
	
protected:
	typedef void (ZLQmlDialogManager::*DialogRequestedSignal)(QObject *);
	
	class Event : public QEvent {
	public:
		template <typename Method>
		Event(QObject *o, const ZLQmlDialogManager *p, Method m);
		virtual ~Event();
		
		static Type eventType();

	private:
		QWeakPointer<QObject> object;
		QWeakPointer<ZLQmlDialogManager> parent;
		DialogRequestedSignal method;
	};
};

#endif /* __ZLQTDIALOGMANAGER_H__ */
