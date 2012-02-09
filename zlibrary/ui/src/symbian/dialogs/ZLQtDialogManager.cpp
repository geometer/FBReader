#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QClipboard>
#include <QtGui/QDesktopWidget>
#include <QtCore/QStringList>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

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

        return new ZLQtDialog(resource()[key], getParentWidget());
}

shared_ptr<ZLOptionsDialog> ZLQtDialogManager::createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool) const {
        return new ZLQtOptionsDialog(resource()[key], applyAction, getParentWidget());
}

shared_ptr<ZLOpenFileDialog> ZLQtDialogManager::createOpenFileDialog(const ZLResourceKey &key, const std::string &directoryPath, const std::string &filePath, const ZLOpenFileDialog::Filter &filter) const {
        return new ZLQtOpenFileDialog(dialogTitle(key), directoryPath, filePath, filter, getParentWidget());
}

shared_ptr<ZLTreeDialog> ZLQtDialogManager::createTreeDialog() const {
        return new ZLQtTreeDialog(getParentWidget());
}

shared_ptr<ZLDialogContent> ZLQtDialogManager::createContent(const ZLResourceKey &key) const {
        return new ZLQtDialogContent(resource()[key]);
}

void ZLQtDialogManager::informationBox(const std::string &title, const std::string &message) const {
        QMessageBox::information(getParentWidget(), ::qtString(title), ::qtString(message), ::qtButtonName(OK_BUTTON));
}

void ZLQtDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
        QMessageBox::critical(getParentWidget(), ::qtString(dialogTitle(key)), ::qtString(message), ::qtButtonName(OK_BUTTON));
}

int ZLQtDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
        Q_UNUSED(button0);
        Q_UNUSED(button1);
        Q_UNUSED(button2);
//        //TODO reimplement this, due to errors on Symbian (how to show 3 buttons as soft buttons?)
        // button2, button0, button1 -- because on symbian, there's no third button (middle button is missed)
        // we don't show button 1
        int result = QMessageBox::question(getParentWidget(), ::qtString(dialogTitle(key)), ::qtString(message), QMessageBox::Ok, QMessageBox::Cancel );
        if (result == QMessageBox::Ok) {
            return 1;
        }
        return 2;
}

shared_ptr<ZLProgressDialog> ZLQtDialogManager::createProgressDialog(const ZLResourceKey &key) const {
	return new ZLQtProgressDialog(key);
}

bool ZLQtDialogManager::isClipboardSupported(ClipboardType type) const {
        Q_UNUSED(type);
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

QWidget* ZLQtDialogManager::getParentWidget() const {
    QWidget* parent = qApp->activeWindow();
    if (parent == 0) {
        parent = myApplicationWindow;
    }
    qDebug() << Q_FUNC_INFO << parent;
    return parent;
}
