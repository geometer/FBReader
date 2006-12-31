#ifndef __SCENEINFODIALOG_H__
#define __SCENEINFODIALOG_H__

#include <ZLApplication.h>

class GeometricCalculator;
class ZLOptionsDialog;

class ShowInfoDialogAction : public ZLApplication::Action {

public:
	ShowInfoDialogAction(GeometricCalculator &calculator);
	void run();

private:
	void createInfoTab(ZLOptionsDialog &dialog);	
	void createDescriptionTab(ZLOptionsDialog &dialog);	

private:
	GeometricCalculator &myCalculator;
};

#endif /* __SCENEINFODIALOG_H__ */
