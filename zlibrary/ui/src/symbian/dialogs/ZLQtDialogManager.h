#ifndef __ZLQTDIALOGMANAGER_H__
#define __ZLQTDIALOGMANAGER_H__

#include <ZLDialogManager.h>
#include "ZLQtOptionsDialog.h"

class QWidget;

class ZLQtDialogManager : public ZLDialogManager {

public:
	static void createInstance() { ourInstance = new ZLQtDialogManager(); }

protected   :
        ZLQtDialogManager() : myStoredWindow(0) {}

public:
	void createApplicationWindow(ZLApplication *application) const;

	shared_ptr<ZLDialog> createDialog(const ZLResourceKey &key) const;
	shared_ptr<ZLOptionsDialog> createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const;
	shared_ptr<ZLOpenFileDialog> createOpenFileDialog(const ZLResourceKey &key, const std::string &directoryPath, const std::string &filePath, const ZLOpenFileDialog::Filter &filter) const;
	shared_ptr<ZLTreeDialog> createTreeDialog() const;
	void informationBox(const std::string &title, const std::string &message) const;
	void errorBox(const ZLResourceKey &key, const std::string &message) const;
	int questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const;
	shared_ptr<ZLProgressDialog> createProgressDialog(const ZLResourceKey &key) const;

	bool isClipboardSupported(ClipboardType type) const;
	void setClipboardText(const std::string &text, ClipboardType type) const;
	void setClipboardImage(const ZLImageData &imageData, ClipboardType type) const;
	bool openURL(const std::string& url) const;

protected:
	mutable QWidget *myStoredWindow;
};

#endif /* __ZLQTDIALOGMANAGER_H__ */
