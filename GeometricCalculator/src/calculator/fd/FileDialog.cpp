#include "FileDialog.h"
#include "FileDialogListView.h"

#include <qlineedit.h>
#include <qvbox.h>
//#include <qpe/timestring.h>

FileDialog::FileDialog(const char *caption, const char *initPath, bool saveDialog, const char *filter) : QDialog(0, caption) {
	myPath.setPath(initPath);
	myFilter = filter;

	myMainBox = new QVBox(this);
	if (saveDialog) {
		myEditString = new QLineEdit(myMainBox);
	} else {
		myEditString = NULL;
	}

	myListView = new FileDialogListView(myMainBox);

	if (saveDialog) {
  	connect(myEditString, SIGNAL(returnPressed()),this, SLOT(runEditLine()));
  	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(selectItem(QListViewItem*)));
  	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(selectItem(QListViewItem*)));
	} else {
  	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(runItem(QListViewItem*)));
  	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(runItem(QListViewItem*)));
	}

	updateListView();
}

QString FileDialog::getOpenFileName(const char *caption, const char *initPath, const char *filter) {
	FileDialog fileDialog(caption, initPath, false, filter);
	if (fileDialog.exec() != Accepted) {
		return QString::null;
	}
	FileDialogItem *item = (FileDialogItem*)fileDialog.myListView->selectedItem();
	if ((item == NULL) || (item->isDir())) {
		return QString::null;
	}
	return fileDialog.myPath.absPath() + "/" + item->name();
}

QString FileDialog::getSaveFileName(const char *caption, const char *initPath, const char *filter) {
	FileDialog fileDialog(caption, initPath, true, filter);
	if (fileDialog.exec() != Accepted) {
		return QString::null;
	}
	return fileDialog.myEditString->text();
}

void FileDialog::runEditLine()
{
	QString text = myEditString->text();
	if (!text.startsWith("/")) {
		text = myPath.absPath() + "/" + text;
	}

	QFileInfo fileInfo(text);
	if (fileInfo.isDir()) {
		myPath.setPath(text);
		if (text[text.length() - 1] != '/') {
			text += "/";
		}
		myEditString->setText(text);
  	updateListView();
	} else if (fileInfo.dir().exists()) {
		myEditString->setText(text);
		accept();
	}
}

void FileDialog::resizeEvent(QResizeEvent *event) {
	if (event != NULL) {
  	myMainBox->resize(event->size());
	}
}

void FileDialog::updateListView() {
  if (!myPath.exists())
		return;

	myListView->clear();

	QListViewItem *item = NULL;
	if (myPath.absPath() != "/") {
	  item = new FileDialogDirItem(myListView, NULL, "..");
	}

	for (QFileInfoListIterator iterator = *myPath.entryInfoList(QDir::Dirs); iterator.current() != NULL; ++iterator) {
		QFileInfo *fileInfo = iterator.current();
		QString fileName = fileInfo->fileName();

		if ((fileName != ".") && (fileName != "..")) {
	 		item = new FileDialogDirItem(myListView, item, fileName);
		}
	}

	for (QFileInfoListIterator iterator = *myPath.entryInfoList(myFilter, QDir::Files); iterator.current() != NULL; ++iterator) {
		QFileInfo *fileInfo = iterator.current();
		item = new FileDialogFileItem(myListView, item, fileInfo->fileName(), fileInfo->lastModified(), fileInfo->size());
	}
}

void FileDialog::selectItem(QListViewItem *item) {
	if (item == NULL) {
		return;
	}

	if (!myPath.exists()) {
		return;
	}

	FileDialogItem *dialogItem = (FileDialogItem*)item;

	if (dialogItem->isDir()) {
		if (dialogItem->name() == "..") {
			myPath.cdUp();
		} else {
			myPath.cd(dialogItem->name());
		}
		updateListView();
		myEditString->setText(myPath.absPath() + "/");
	} else {
		myEditString->setText(myPath.absPath() + "/" + dialogItem->name());
	}
}

void FileDialog::runItem(QListViewItem *item) {
	if (item == NULL) {
		return;
	}

	if (!myPath.exists()) {
		return;
	}

	FileDialogItem *dialogItem = (FileDialogItem*)item;

	if (dialogItem->isDir()) {
		if (dialogItem->name() == "..") {
			myPath.cdUp();
		} else {
			myPath.cd(dialogItem->name());
		}
		updateListView();
	} else {
		accept();
	}
}
