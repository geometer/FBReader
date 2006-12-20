#ifndef __FILEDIALOG_H__
#define __FILEDIALOG_H__

#include <qdir.h>
#include <qdialog.h>

class QVBox;
class QLineEdit;
class FileDialogListView;
class QListViewItem;

class FileDialog : public QDialog {
	Q_OBJECT

public:
	static QString getOpenFileName(const char *caption, const char *initPath, const char *filter = NULL);
	static QString getSaveFileName(const char *caption, const char *initPath, const char *filter = NULL);

private:
	FileDialog(const char *caption, const char *initPath, bool saveDialog, const char *filter); 

	void updateListView();

private slots:
	void resizeEvent(QResizeEvent *event);
	void selectItem(QListViewItem *item);
	void runItem(QListViewItem *item);
	void runEditLine();

private:
	QString myFilter;
	QDir myPath;

	FileDialogListView *myListView;
	QVBox *myMainBox;
	QLineEdit *myEditString;
};

#endif /* __FILEDIALOG_H__ */
