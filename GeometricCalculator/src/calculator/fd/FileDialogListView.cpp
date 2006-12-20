#include "FileDialogListView.h"

#include <qpixmap.h>
//#include <qpe/timestring.h>
//#include <qpe/resource.h>

static QPixmap *folderPixmap = NULL;
static QPixmap *sceneFilePixmap = NULL;

FileDialogListView::FileDialogListView(QWidget *widget) : QListView(widget) {
	addColumn("Name");
	addColumn("Modified");
	addColumn("Size");

	myLastSortColumn = 0;
	myLastIncreasingSort = false;
}

void FileDialogListView::resizeContents(int width, int height) {
	QListView::resizeContents(width, height);

	/*
	int fullWidth = 0;
	for (int i = 0; i < 3; i++) {
		fullWidth += columnWidth(i) + 1;
	}
	int realWidth = size().width() - 4;
	QScrollBar *bar = verticalScrollBar();
	if (bar->isVisible()) {
		realWidth -= bar->width();
	}
	for (int i = 0; i < 3; i++) {
		setColumnWidth(i, (columnWidth(i) + 1) * realWidth / fullWidth);
	}

	QListView::resizeContents(width, height);
	*/
}

void FileDialogListView::setSorting(int column, bool increasing) {
	sort(column);
}

void FileDialogListView::sort(int column) {
	if ((column < 0) && (column >= columns())) {
		return;
	}

	if (column == myLastSortColumn) {
		myLastIncreasingSort = !myLastIncreasingSort;
	} else {
		myLastIncreasingSort = true;
	}
	myLastSortColumn = column;

	int N = childCount();
	bool noChanges;
	do {
		noChanges = true;
		FileDialogItem *current = (FileDialogItem*)firstChild();
		FileDialogItem *next = (FileDialogItem*)current->nextSibling();
		for (int i = 0; i < N - 1; i++) {
			if (current->compare(next, column, myLastIncreasingSort) == 1) {
				current->moveItem(next);
				noChanges = false;
			}
			current = next;
			next = (FileDialogItem*)current->nextSibling();
		}
	} while (!noChanges);
}

FileDialogItem::FileDialogItem(QListView *listView, QListViewItem *previous, const QString name, QDateTime modified, int size) : QListViewItem(listView, previous, name, (modified == QDateTime()) ? QString("") : "XXX", (size == -1) ? QString("") : QString("%1").arg(size)) {
	myModified = modified;
	mySize = size;
}

QString FileDialogItem::name() {
	return text(0);
}

int FileDialogItem::compare(FileDialogItem *item, int column, bool increasingSort) {
	if (this == item) {
		return 0;
	}
	if (isDir() && !item->isDir()) {
		return -1;
	}
	if (!isDir() && item->isDir()) {
		return 1;
	}

	switch (column) {
		case 1:
			if (myModified < item->myModified) {
				return increasingSort ? -1 : 1;
			}
			if (myModified > item->myModified) {
				return increasingSort ? 1 : -1;
			}
			break;
		case 2:
			if (mySize < item->mySize) {
				return increasingSort ? -1 : 1;
			}
			if (mySize > item->mySize) {
				return increasingSort ? 1 : -1;
			}
			break;
	}

	if (name() < item->name()) {
		return increasingSort ? -1 : 1;
	}
	if (name() > item->name()) {
		return increasingSort ? 1 : -1;
	}
	return 0;
}

FileDialogDirItem::FileDialogDirItem(QListView *listView, QListViewItem *previous, const QString name) : FileDialogItem(listView, previous, name, QDateTime(), -1) {
	if (folderPixmap == NULL) {
		folderPixmap = new QPixmap();//Resource::loadPixmap("GeometricCalculator/folder"));
	}
	setPixmap(0, *folderPixmap);
}

bool FileDialogDirItem::isDir() {
	return true;
}

FileDialogFileItem::FileDialogFileItem(QListView *listView, QListViewItem *previous, const QString name, QDateTime modified, int size) : FileDialogItem(listView, previous, name, modified, size) {
	if (sceneFilePixmap == NULL) {
		sceneFilePixmap = new QPixmap();//Resource::loadPixmap("GeometricCalculator/scene_file"));
	}
	setPixmap(0, *sceneFilePixmap);
}

bool FileDialogFileItem::isDir() {
	return false;
}
