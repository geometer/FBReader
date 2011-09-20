#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>
#include <QtGui/QAction>
#include <QtGui/QPushButton>

#include <ZLApplication.h>

#include "ZLQtOpenFileDialog.h"

#include "ZLQtUtil.h"


static const std::string EMPTY_MESSAGE("emptyMessage");


ZLQtOpenFileDialog::ZLQtOpenFileDialog(const std::string &title, const std::string &directoryPath, const std::string &filePath, const Filter &filter)
	: myFilter(filter) {
	myDialog = new DrillDownMenuDialog(qApp->activeWindow());
	mySelectDirsOnlyMode = false;

//	myDialog->setWindowTitle(QString::fromUtf8(title.c_str()));
//	myDialog->setDirectory(QString::fromUtf8(directoryPath.c_str()));
//	myDialog->selectFile(QString::fromUtf8(filePath.c_str()));hg
}

void ZLQtOpenFileDialog::selectDirectoriesOnly() {
	mySelectDirsOnlyMode = true;
}

bool ZLQtOpenFileDialog::run() {
	return runInternal();
}

ZLQtOpenFileDialog::~ZLQtOpenFileDialog() {
	delete myDialog;
}

bool ZLQtOpenFileDialog::runInternal() {
	DrillDownMenu* myRootMenu = new DrillDownMenu;

	QFileInfoList list = QDir::drives();
	for (int i=0; i<list.size(); ++i) {
		QFileInfo info = list[i];
		myRootMenu->addItem( ::stdString(info.filePath()), new DirAction(this, myDialog, info.filePath(), myFilter ));
	}

	myDialog->showDrillDownMenu(myRootMenu);

	if (mySelectDirsOnlyMode == true) {
		const ZLResource& select = ZLResource::resource("dialog")["button"]["selectDir"];
		QAction* action = new QAction(select.value().c_str(),myDialog);
		action->setSoftKeyRole( QAction::PositiveSoftKey );
		QObject::connect(action, SIGNAL(triggered()), myDialog, SLOT(accept()));
		myDialog->addAction( action );

#ifndef 	__SYMBIAN__
		QPushButton* button = new QPushButton( select.value().c_str() );
		QObject::connect(button, SIGNAL(clicked()), myDialog, SLOT(accept()));
		myDialog->layout()->addWidget(button);
#endif

	}

	return myDialog->run();
}



std::string ZLQtOpenFileDialog::filePath() const {
	//	QStringList paths = myDialog->selectedFiles();
	//		return paths.size() > 0 ? (const char*)paths[0].toUtf8() : std::string();
	//QMessageBox::information(0, "", "MyFilePath: " + myFilePath, QMessageBox::Ok);

	return  ::stdString(myFilePath);
}

std::string ZLQtOpenFileDialog::directoryPath() const {
	//return (const char*)myDialog->directory().absolutePath().toUtf8();
	//QMessageBox::information(0, "", "directoryPath: " + myDirectoryPath, QMessageBox::Ok);
	return  ::stdString(myDirectoryPath);
}


FileAction::FileAction(ZLQtOpenFileDialog* openFileDialog, DrillDownMenuDialog* dialog, QString filePath): MenuAction(dialog),myOpenFileDialog(openFileDialog), myFile(filePath) {
}

void FileAction::run() {
		myOpenFileDialog->myFilePath = myFile;
		myDialog->accept();
	}

DirAction::DirAction(ZLQtOpenFileDialog* openFileDialog, DrillDownMenuDialog* dialog, QString dirPath, const ZLOpenFileDialog::Filter &filter) : MenuAction(dialog), myOpenFileDialog(openFileDialog), myDir(dirPath), myFilter(filter), myMenu(0) {
}

void DirAction::run() {
	myOpenFileDialog->myDirectoryPath =  myDir.absolutePath();
	myDir.setSorting(QDir::DirsFirst);
	myDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
	if (myMenu == 0) {
		myMenu = new DrillDownMenu;
		run_init();
	}
	myDialog->showDrillDownMenu(myMenu);
}

void DirAction::run_init() {
	QFileInfoList list = myDir.entryInfoList();
	for (int i=0; i<list.size(); ++i) {
		QFileInfo info = list[i];
		if ( info.isDir() ) {
			QDir newDir(myDir);
			newDir.cd(info.fileName());
			std::string image = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + "folder.png";
			ZLFSManager::Instance().normalize(image);
			QIcon icon(image.c_str());
			myMenu->addItem(icon, ::stdString(info.fileName()), new DirAction(myOpenFileDialog, myDialog,  newDir.absolutePath(),myFilter));
		} else if (myOpenFileDialog->mySelectDirsOnlyMode == false &&  myFilter.accepts(ZLFile(::stdString(info.absoluteFilePath())))) {
			std::string image = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + "fb2.png";
			ZLFSManager::Instance().normalize(image);
			QIcon icon(image.c_str());
			myMenu->addItem(icon, ::stdString(info.fileName()), new FileAction(myOpenFileDialog, myDialog, info.absoluteFilePath() ));
		}
	}

	if (myMenu->count() == 0) {
		std::string message;
		if (myOpenFileDialog->mySelectDirsOnlyMode == true) {
			message = ZLResource::resource("dialog")["addBooksDirDialog"][EMPTY_MESSAGE].value();
		} else {
			message = ZLResource::resource("dialog")["addFileDialog"][EMPTY_MESSAGE].value();
		}
		myMenu->setMessage(message);
	}

}
