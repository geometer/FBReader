/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <QtCore/QLocale>

#include <ZLApplication.h>
#include <ZLibrary.h>
#include <ZLLanguageUtil.h>

#include "../../../../core/src/unix/library/ZLibraryImplementation.h"

#include "../filesystem/ZLQtFSManager.h"
#include "../time/ZLQtTime.h"
#include "../dialogs/ZLQmlDialogManager.h"
#include "../image/ZLQtImageManager.h"
#include "../view/ZLQmlPaintContext.h"
#include "../network/ZLQtNetworkManager.h"
#include "../message/ZLSymbianMessage.h"
#include "../../../../core/src/util/ZLKeyUtil.h"
#include "../../../../core/src/unix/xmlconfig/XMLConfig.h"
#include "../../../../core/src/unix/iconv/IConvEncodingConverter.h"
//#include "../../../../core/src/unix/curl/ZLCurlNetworkManager.h"

#include <QtCore/QDebug>

const std::string ZLibrary::FileNameDelimiter("\\");
const std::string ZLibrary::PathDelimiter(";");
const std::string ZLibrary::EndOfLine("\n");

std::string ZLibrary::ZLibraryDirectory() {
    std::string s = ourZLibraryDirectory;
    ZLFSManager::Instance().normalize(s);
    return s;
}
std::string ZLibrary::ApplicationName() { return ourApplicationName; }
std::string ZLibrary::ImageDirectory() {
    std::string s(ourImageDirectory);
    ZLFSManager::Instance().normalize(s);
    return s;
}
std::string ZLibrary::ApplicationImageDirectory() {
    std::string s(ourApplicationImageDirectory);
    ZLFSManager::Instance().normalize(s);
    return s;
}
std::string ZLibrary::ApplicationDirectory() {
    std::string s(ourApplicationDirectory);
    ZLFSManager::Instance().normalize(s);
    return s;
}
std::string ZLibrary::ApplicationWritableDirectory() {
    std::string s(ourApplicationWritableDirectory);
    ZLFSManager::Instance().normalize(s);
    return s;
}
std::string ZLibrary::DefaultFilesPathPrefix() {
    std::string s(ourDefaultFilesPathPrefix);
    ZLFSManager::Instance().normalize(s);
    return s;
}


void ZLibrary::initLocale() {
		std::string localeQtStdString = QLocale::system().name().toStdString();
		const char *localeQt = localeQtStdString.c_str();
		const char* locale = localeQt;
	if (locale != 0) {
		std::string sLocale = locale;
		const int dotIndex = sLocale.find('.');
		if (dotIndex != -1) {
			sLocale = sLocale.substr(0, dotIndex);
		}
		const int dashIndex = std::min(sLocale.find('_'), sLocale.find('-'));
		if (dashIndex == -1) {
			ourLanguage = sLocale;
		} else {
			ourLanguage = sLocale.substr(0, dashIndex);
			ourCountry = sLocale.substr(dashIndex + 1);
			if ((ourLanguage == "es") && (ourCountry != "ES")) {
				ourCountry = "LA";
			}
		}
	}

}

void myMessageOutput(QtMsgType type, const char *msg)
 {
         switch (type) {
         case QtDebugMsg:
                 fprintf(stdout, "Debug: %s\n", msg);
                 break;
         case QtWarningMsg:
                 fprintf(stdout, "Warning: %s\n", msg);
                 break;
         case QtCriticalMsg:
                 fprintf(stdout, "Critical: %s\n", msg);
                 break;
         case QtFatalMsg:
                 fprintf(stdout, "Fatal: %s\n", msg);
                 //abort();
         }
         fflush(stdout);
 }


ZLibraryImplementation *ZLibraryImplementation::Instance = 0;

ZLibraryImplementation::ZLibraryImplementation() {
	Instance = this;
}

ZLibraryImplementation::~ZLibraryImplementation() {
}

bool ZLibrary:: init(int &argc, char **&argv) {
        freopen("E:\\fbreader-log.txt", "w", stdout);
        fprintf(stdout,"\n");

	initLibrary();

	if (ZLibraryImplementation::Instance == 0) {
		return false;
	}

	ZLibraryImplementation::Instance->init(argc, argv);
	return true;
}

ZLPaintContext *ZLibrary::createContext() {
	return ZLibraryImplementation::Instance->createContext();
}

void ZLibrary::run(ZLApplication *application) {
		ZLibraryImplementation::Instance->run(application);
}

class ZLQmlLibraryImplementation : public ZLibraryImplementation {

private:
	void init(int &argc, char **&argv);
	ZLPaintContext *createContext();
	void run(ZLApplication *application);
};

void initLibrary() {
	new ZLQmlLibraryImplementation();
}

void ZLQmlLibraryImplementation::init(int &argc, char **&argv) {
        new QApplication(argc, argv);
        qInstallMsgHandler(myMessageOutput);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

	ZLibrary::parseArguments(argc, argv);

	XMLConfigManager::createInstance();
	ZLQtTimeManager::createInstance();
	ZLQtFSManager::createInstance();
	ZLQmlDialogManager::createInstance();
	ZLSymbianCommunicationManager::createInstance();
	ZLQtImageManager::createInstance();
	//ZLEncodingCollection::Instance().registerProvider(new IConvEncodingConverterProvider());
	ZLQtNetworkManager::createInstance();
}

ZLPaintContext *ZLQmlLibraryImplementation::createContext() {
	return new ZLQmlPaintContext();
}

void ZLQmlLibraryImplementation::run(ZLApplication *application) {
	if (ZLLanguageUtil::isRTLLanguage(ZLibrary::Language())) {
		qApp->setLayoutDirection(Qt::RightToLeft);
	}
	static_cast<ZLQtNetworkManager&>(ZLNetworkManager::Instance()).initPaths();
	ZLDialogManager::Instance().createApplicationWindow(application);
	application->initWindow();
	qApp->exec();
	delete application;
}
