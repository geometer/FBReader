#include <qvbox.h>
#include <qlistview.h>

#include <qpe/resource.h>

#include <abstract/ZLFSDir.h>
#include <abstract/ZLZipDir.h>
#include <abstract/ZLStringUtil.h>


#include "QOpenFileDialog.h"

static QPixmap *folderPixmap = NULL;
static QPixmap *htmlFilePixmap = NULL;
static QPixmap *zipFilePixmap = NULL;
static QPixmap *unknownFilePixmap = NULL;

QOpenFileDialogItem::QOpenFileDialogItem(QListView *listView, QListViewItem *previous, const QString name, bool dir) : QListViewItem(listView, previous, name), myIsDir(dir) {
	if (folderPixmap == NULL) {
		folderPixmap = new QPixmap(Resource::loadPixmap("FBReader/folder"));
		htmlFilePixmap = new QPixmap(Resource::loadPixmap("FBReader/html"));
		zipFilePixmap = new QPixmap(Resource::loadPixmap("FBReader/zipfolder"));
		unknownFilePixmap = new QPixmap(Resource::loadPixmap("FBReader/unknown"));
	}
	if (dir) {
		setPixmap(0, *folderPixmap);
	} else if (ZLStringUtil::stringEndsWith(name.ascii(), ".html")) {
		setPixmap(0, *htmlFilePixmap);
	} else if (ZLStringUtil::stringEndsWith(name.ascii(), ".zip")) {
		setPixmap(0, *zipFilePixmap);
	} else {
		setPixmap(0, *unknownFilePixmap);
	}
}

QString QOpenFileDialogItem::name() {
	return text(0);
}

QOpenFileDialog::QOpenFileDialog(const char *caption, const char *initPath) : FullScreenDialog(caption) {
	resize(600, 600);
	
	myCurrentDir = new ZLFSDir(initPath);

	myMainBox = new QVBox(this);

	myListView = new QListView(myMainBox);
	myListView->addColumn("File Name");

 	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(runItem(QListViewItem*)));
 	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(runItem(QListViewItem*)));

	updateListView();
}

std::string QOpenFileDialog::getOpenFileName(const char *caption, const char *initPath) {
	QOpenFileDialog fileDialog(caption, initPath);
	if (fileDialog.exec() != Accepted) {
		return std::string();
	}
	QOpenFileDialogItem *item = (QOpenFileDialogItem*)fileDialog.myListView->selectedItem();
	if ((item == NULL) || (item->isDir())) {
		return std::string();
	}
	return fileDialog.myCurrentDir->name() + fileDialog.myCurrentDir->delimiter() + item->name().ascii();
}

bool QOpenFileDialog::isDirectoryVisible(const std::string &name) {
	return (name.length() > 0) && (name[0] != '.');
}

bool QOpenFileDialog::isFileVisible(const std::string &name) {
	if ((name.length() == 0) || (name[0] == '.')) {
		return false;
	}
	return
		ZLStringUtil::stringEndsWith(name, ".html") ||
		ZLStringUtil::stringEndsWith(name, ".zip") ||
		ZLStringUtil::stringEndsWith(name, ".fb2");
}

void QOpenFileDialog::resizeEvent(QResizeEvent *event) {
	if (event != NULL) {
  	myMainBox->resize(event->size());
	}
}

void QOpenFileDialog::updateListView() {
	myListView->clear();

	QListViewItem *item = NULL;
	if (myCurrentDir->name() != "/") {
	  item = new QOpenFileDialogItem(myListView, NULL, "..", true);
	}

	std::vector<std::string> dirNames;
	myCurrentDir->collectSubDirs(dirNames);
	for (std::vector<std::string>::const_iterator it = dirNames.begin(); it != dirNames.end(); it++) {
		if (isDirectoryVisible(*it)) {
	 		item = new QOpenFileDialogItem(myListView, item, it->c_str(), true);
		}
	}

	std::vector<std::string> fileNames;
	myCurrentDir->collectRegularFiles(fileNames);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++) {
		if (isFileVisible(*it)) {
	 		item = new QOpenFileDialogItem(myListView, item, it->c_str(), false);
		}
	}
}

void QOpenFileDialog::runItem(QListViewItem *item) {
	if (item == NULL) {
		return;
	}

	QOpenFileDialogItem *dialogItem = (QOpenFileDialogItem*)item;

	if (dialogItem->isDir()) {
		std::string name = myCurrentDir->name();
		if (dialogItem->name() == "..") {
			name = name.substr(0, name.rfind('/'));
		} else {
			name += std::string("/") + dialogItem->name().ascii();
		}
		delete myCurrentDir;
		myCurrentDir = new ZLFSDir(name);
		updateListView();
	} else if (ZLStringUtil::stringEndsWith(dialogItem->name().ascii(), ".zip")) {
		std::string name = myCurrentDir->name();
		name += std::string("/") + dialogItem->name().ascii();
		delete myCurrentDir;
		myCurrentDir = new ZLZipDir(name);
		updateListView();
	} else {
		accept();
	}
}
