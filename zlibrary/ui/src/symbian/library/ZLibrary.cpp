#include <locale.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>

#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QLocale>
#include <QS60Style>

#include <ZLApplication.h>
#include <ZLibrary.h>
#include <ZLLanguageUtil.h>
#include <ZLStringUtil.h>

#include "../../../../core/src/unix/library/ZLibraryImplementation.h"

#include "../filesystem/ZLQtFSManager.h"
#include "../time/ZLQtTime.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../image/ZLQtImageManager.h"
#include "../view/ZLQtPaintContext.h"
#include "../message/ZLSymbianMessage.h"
#include "../../../../core/src/util/ZLKeyUtil.h"
#include "../../../../core/src/unix/xmlconfig/XMLConfig.h"
#include "../../../../core/src/unix/iconv/IConvEncodingConverter.h"
#include "../network/ZLQtNetworkManager.h"

#include "../../../../core/src/unix/library/ZLibraryImplementation.h"

const std::string ZLibrary::FileNameDelimiter("\\");
const std::string ZLibrary::PathDelimiter(";");
const std::string ZLibrary::EndOfLine("\n");         // don't know exactly what should be here

void ZLibrary::initLocale() {
		std::string localeQtStdString = QLocale::system().name().toStdString();
		const char *localeQt = localeQtStdString.c_str();
		const char *localeSystem = setlocale(LC_MESSAGES, "");
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

ZLibraryImplementation *ZLibraryImplementation::Instance = 0;

ZLibraryImplementation::ZLibraryImplementation() {
	Instance = this;
}

ZLibraryImplementation::~ZLibraryImplementation() {
}

bool ZLibrary:: init(int &argc, char **&argv) {
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


class ZLQtLibraryImplementation : public ZLibraryImplementation {

private:
	void init(int &argc, char **&argv);
	ZLPaintContext *createContext();
	void run(ZLApplication *application);
};

void initLibrary() {
	new ZLQtLibraryImplementation();
}

void ZLQtLibraryImplementation::init(int &argc, char **&argv) {
	new QApplication(argc, argv);
        //qApp->setStyle( new QS60Style() );
        ZLibrary::parseArguments(argc, argv);
        XMLConfigManager::createInstance();
        ZLQtTimeManager::createInstance();
        ZLQtFSManager::createInstance();
        ZLQtDialogManager::createInstance();
		ZLSymbianCommunicationManager::createInstance();
        ZLQtImageManager::createInstance();
        ZLEncodingCollection::Instance().registerProvider(new IConvEncodingConverterProvider());
		ZLQtNetworkManager::createInstance();

		ZLKeyUtil::setKeyNamesFileName("keynames.xml");
}

ZLPaintContext *ZLQtLibraryImplementation::createContext() {
        return new ZLQtPaintContext();
}

void ZLQtLibraryImplementation::run(ZLApplication *application) {
        if (ZLLanguageUtil::isRTLLanguage(ZLibrary::Language())) {
                qApp->setLayoutDirection(Qt::RightToLeft);
        }
        ZLDialogManager::Instance().createApplicationWindow(application);
	application->initWindow();
        qApp->exec(); //static_cast<QApplication *>(QCoreApplication::instance())->exec()
        delete application;
}
