#ifndef __SCENEINFODIALOG_H__
#define __SCENEINFODIALOG_H__

#include <ZLApplication.h>

class GeometricCalculator;
class ZLOptionsDialog;

class ShowInfoDialogAction : public ZLApplication::Action {

public:
	ShowInfoDialogAction(GeometricCalculator &window);
	void run();

private:
	void createInfoTab(ZLOptionsDialog &dialog);	

private:
	GeometricCalculator &myWindow;
};

#endif /* __SCENEINFODIALOG_H__ */
