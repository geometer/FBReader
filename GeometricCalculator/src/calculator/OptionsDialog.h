#ifndef __OPTIONSDIALOG_H__
#define __OPTIONSDIALOG_H__

#include <ZLApplication.h>

class GeometricCalculator;
class ZLOptionsDialog;

class ShowOptionsDialogAction : public ZLApplication::Action {

public:
	ShowOptionsDialogAction(GeometricCalculator &calculator);
	void run();

private:
	void createViewTab(ZLOptionsDialog &dialog);	
	void createDrawingTab(ZLOptionsDialog &dialog);	
	void createPointTab(ZLOptionsDialog &dialog);	
	void createColorsTab(ZLOptionsDialog &dialog);	
	void createKeysTab(ZLOptionsDialog &dialog);	

private:
	GeometricCalculator &myCalculator;
};

#endif /* __OPTIONSDIALOG_H__ */
