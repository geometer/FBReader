/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include <QtCore/QFile>

#include <ZLApplication.h>
#include <ZLibrary.h>
#include <ZLLanguageUtil.h>

#include "../../../../core/src/unix/library/ZLibraryImplementation.h"

#include "../filesystem/ZLQtFSManager.h"
#include "../time/ZLQtTime.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../image/ZLQtImageManager.h"
#include "../view/ZLQtPaintContext.h"
#include "../network/ZLQtNetworkManager.h"
#include "../../unix/message/ZLUnixMessage.h"
#include "../../../../core/src/util/ZLKeyUtil.h"
#include "../../../../core/src/unix/xmlconfig/XMLConfig.h"
#include "../../../../core/src/unix/iconv/IConvEncodingConverter.h"

class ZLQtLibraryImplementation : public ZLibraryImplementation {

private:
	void init(int &argc, char **&argv);
	void setStylesheet(const std::string &filename);
	ZLPaintContext *createContext();
	void run(ZLApplication *application);
};

void initLibrary() {
	new ZLQtLibraryImplementation();
}

void ZLQtLibraryImplementation::init(int &argc, char **&argv) {
	new QApplication(argc, argv);

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

	ZLibrary::parseArguments(argc, argv);

	XMLConfigManager::createInstance();
	ZLQtTimeManager::createInstance();
	ZLQtFSManager::createInstance();
	ZLQtDialogManager::createInstance();
	ZLUnixCommunicationManager::createInstance();
	ZLQtImageManager::createInstance();
	ZLEncodingCollection::Instance().registerProvider(new IConvEncodingConverterProvider());
	ZLQtNetworkManager::createInstance();

	setStylesheet("style.qss");
	ZLKeyUtil::setKeyNamesFileName("keynames-qt4.xml");
}

void ZLQtLibraryImplementation::setStylesheet(const std::string &filename) {
	static const std::string stylesheetPath = ZLibrary::ZLibraryDirectory() + ZLibrary::FileNameDelimiter + filename;
	QFile file(QString::fromStdString(ZLFile(stylesheetPath).path()));
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	qApp->setStyleSheet(styleSheet);
}

ZLPaintContext *ZLQtLibraryImplementation::createContext() {
	return new ZLQtPaintContext();
}

void ZLQtLibraryImplementation::run(ZLApplication *application) {
	if (ZLLanguageUtil::isRTLLanguage(ZLibrary::Language())) {
		qApp->setLayoutDirection(Qt::RightToLeft);
	}
	static_cast<ZLQtNetworkManager&>(ZLNetworkManager::Instance()).initPaths();
	ZLDialogManager::Instance().createApplicationWindow(application);
	application->initWindow();
	qApp->exec();
	static_cast<ZLQtDialogManager&>(ZLDialogManager::Instance()).notifyApplicationWindowDeleted();
	delete application;
}
