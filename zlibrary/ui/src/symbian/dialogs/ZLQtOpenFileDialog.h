#ifndef __ZLQTOPENFILEDIALOG_H__
#define __ZLQTOPENFILEDIALOG_H__

#include <QDir>

#include "../../../../core/src/desktop/dialogs/ZLDesktopOpenFileDialog.h"
#include "../menu/DrillDownMenu.h"
#include "../actions/DrillDownMenuActions.h"

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLOpenFileDialog.h>

#include "../filesystem/ZLQtFSManager.h"

class ZLQtOpenFileDialog : public ZLDesktopOpenFileDialog {

public:
	ZLQtOpenFileDialog(const std::string &title, const std::string &directoryPath, const std::string &filePath, const Filter &filter);
	~ZLQtOpenFileDialog();

	bool runInternal();
	std::string filePath() const;
	std::string directoryPath() const;

	void setPosition(int x, int y);
	void setSize(int width, int height);
	int x() const;
	int y() const;
	int width() const;
	int height() const;

public:
	void selectDirectoriesOnly();


private:
	QString myDirectoryPath;
	QString myFilePath;
	bool mySelectDirsOnlyMode;
	DrillDownMenuDialog *myDialog;
	const ZLOpenFileDialog::Filter &myFilter;

	friend class FileAction;
	friend class DirAction;
};


class FileAction : public MenuAction {
public:
	FileAction(ZLQtOpenFileDialog* openFileDialog, DrillDownMenuDialog* dialog, QString filePath);
	void run();
private:
	QString myFile;
	ZLQtOpenFileDialog* myOpenFileDialog;
};

class DirAction : public MenuAction {
public:
	DirAction(ZLQtOpenFileDialog* openFileDialog, DrillDownMenuDialog* dialog, QString dirPath, const ZLOpenFileDialog::Filter &filter);
	void run();
	void run_init();
private:
	QDir myDir;
	DrillDownMenu* myMenu;
	const ZLOpenFileDialog::Filter &myFilter;
	ZLQtOpenFileDialog* myOpenFileDialog;
};



#endif // __ZLQTOPENFILEDIALOG_H__
