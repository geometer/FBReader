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

#ifndef __ZLQTDIALOGMANAGER_H__
#define __ZLQTDIALOGMANAGER_H__

#include <ZLDialogManager.h>
#include <QtCore/QObject>

class QWidget;

class ZLQmlDialogManager : public QObject, public ZLDialogManager {
	Q_OBJECT

public:
	static void createInstance() { ourInstance = new ZLQmlDialogManager(); }

private:
	ZLQmlDialogManager();

public:
	void createApplicationWindow(ZLApplication *application) const;

	shared_ptr<ZLDialog> createDialog(const ZLResourceKey &key) const;
	shared_ptr<ZLOptionsDialog> createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const;
	shared_ptr<ZLOpenFileDialog> createOpenFileDialog(const ZLResourceKey &key, const std::string &directoryPath, const std::string &filePath, const ZLOpenFileDialog::Filter &filter) const;
	void informationBox(const std::string &title, const std::string &message) const;
	void errorBox(const ZLResourceKey &key, const std::string &message) const;
	int questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const;
	shared_ptr<ZLProgressDialog> createProgressDialog(const ZLResourceKey &key) const;

	bool isClipboardSupported(ClipboardType type) const;
	void setClipboardText(const std::string &text, ClipboardType type) const;
	void setClipboardImage(const ZLImageData &imageData, ClipboardType type) const;

Q_SIGNALS:
	void privateDialogRequested(QObject *object);
	void dialogRequested(QObject *object);
	void privateFileDialogRequested(QObject *object);
	void fileDialogRequested(QObject *object);
	void privateOptionsDialogRequested(QObject *object);
	void optionsDialogRequested(QObject *object);
	void privateInformationBoxRequested(const QString &title, const QString &message, const QString &button);
	void informationBoxRequested(const QString &title, const QString &message, const QString &button);
	void privateErrorBoxRequested(const QString &title, const QString &message, const QString &button);
	void errorBoxRequested(const QString &title, const QString &message, const QString &button);
//	void privateQuestionBoxRequested(QObject *object);
};

#endif /* __ZLQTDIALOGMANAGER_H__ */
