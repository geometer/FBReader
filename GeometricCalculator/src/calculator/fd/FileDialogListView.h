#ifndef __FILEDIALOGLISTVIEW_H__
#define __FILEDIALOGLISTVIEW_H__

#include <qlistview.h>
#include <qdatetime.h>

class FileDialogListView : public QListView {
	Q_OBJECT

	public:
		FileDialogListView(QWidget *widget);
		void setSorting(int column, bool increasing = TRUE);

	private:
		void resizeContents(int width, int height);
		void sort(int column);

	private:
		int myLastSortColumn;
		bool myLastIncreasingSort;
};

class FileDialogItem : public QListViewItem {
	public:
		FileDialogItem(QListView *listView, QListViewItem *previous, const QString name, QDateTime modified, int size);
		virtual bool isDir() = 0;
		QString name();
		int compare(FileDialogItem *item, int column, bool increasingSort);

	private:
		QDateTime myModified;
		int mySize;
};

class FileDialogDirItem : public FileDialogItem {
	public:
		FileDialogDirItem(QListView *listView, QListViewItem *previous, const QString name);
		bool isDir();
};

class FileDialogFileItem : public FileDialogItem {
	public:
		FileDialogFileItem(QListView *listView, QListViewItem *previous, const QString name, QDateTime modified, int size);
		bool isDir();
};

#endif /* __FILEDIALOGLISTVIEW_H__ */
