#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QClipboard>
#include <QtGui/QDesktopWidget>

#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>

#include "ZLQtDialogManager.h"
#include "ZLQtDialog.h"
#include "ZLQtOptionsDialog.h"
#include "ZLQtOpenFileDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtProgressDialog.h"
#include "ZLQtTreeDialog.h"
#include "ZLQtUtil.h"

#include "../image/ZLQtImageManager.h"

shared_ptr<ZLDialog> ZLQtDialogManager::createDialog(const ZLResourceKey &key) const {
	myStoredWindow = qApp->activeWindow();
	return new ZLQtDialog(resource()[key]);
}

shared_ptr<ZLOptionsDialog> ZLQtDialogManager::createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool) const {
	myStoredWindow = qApp->activeWindow();
	return new ZLQtOptionsDialog(resource()["MobileOptionsDialog"]["tab"][key], applyAction);
}

shared_ptr<ZLOpenFileDialog> ZLQtDialogManager::createOpenFileDialog(const ZLResourceKey &key, const std::string &directoryPath, const std::string &filePath, const ZLOpenFileDialog::Filter &filter) const {
	return new ZLQtOpenFileDialog(dialogTitle(key), directoryPath, filePath, filter);
}

shared_ptr<ZLTreeDialog> ZLQtDialogManager::createTreeDialog() const {
	return new ZLQtTreeDialog;
}


void ZLQtDialogManager::informationBox(const std::string &title, const std::string &message) const {
	QWidget *parent = qApp->activeWindow();
	if (parent == 0) {
		parent = myStoredWindow;
	}
	QMessageBox::information(parent, ::qtString(title), ::qtString(message), ::qtButtonName(OK_BUTTON));
}

void ZLQtDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	QWidget *parent = qApp->activeWindow();
	if (parent == 0) {
		parent = myStoredWindow;
	}
	QMessageBox::critical(parent, ::qtString(dialogTitle(key)), ::qtString(message), ::qtButtonName(OK_BUTTON));
}

int ZLQtDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	QWidget *parent = qApp->activeWindow();
	if (parent == 0) {
		parent = myStoredWindow;
	}
	return QMessageBox::question(parent, ::qtString(dialogTitle(key)), ::qtString(message), ::qtButtonName(button0), ::qtButtonName(button1), ::qtButtonName(button2));
}

shared_ptr<ZLProgressDialog> ZLQtDialogManager::createProgressDialog(const ZLResourceKey &key) const {
	return new ZLQtProgressDialog(key);
}

bool ZLQtDialogManager::isClipboardSupported(ClipboardType type) const {
	return true;
}

void ZLQtDialogManager::setClipboardText(const std::string &text, ClipboardType type) const {
	if (!text.empty()) {
		qApp->clipboard()->setText(
			::qtString(text),
			(type == CLIPBOARD_MAIN) ? QClipboard::Clipboard : QClipboard::Selection
		);
	}
}

void ZLQtDialogManager::setClipboardImage(const ZLImageData &imageData, ClipboardType type) const {
	qApp->clipboard()->setImage(
		*((ZLQtImageData&)imageData).image(),
		(type == CLIPBOARD_MAIN) ? QClipboard::Clipboard : QClipboard::Selection
	);
}

bool ZLQtDialogManager::openURL(const std::string& url) const {
	return QDesktopServices::openUrl(QUrl::fromUserInput(QString::fromStdString(url)));
}
