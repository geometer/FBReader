#ifndef __GEOMETRICCALCULATOR_H__
#define __GEOMETRICCALCULATOR_H__

#include <vector>

#include <shared_ptr.h>
#include <ZLView.h>
#include <ZLApplication.h>
#include <ZLKeyBindings.h>

class DiagramView;

class GeometricCalculator : public ZLApplication { 

public:
	static std::string ConfigDirectory();

private:

public:
	GeometricCalculator(const std::string &fileName);
	~GeometricCalculator();

	void initWindow();
	ZLKeyBindings &keyBindings();

private:
	const std::string defaultSceneFileName() const;

private:
	bool saveIfChanged();
	void newScene();
	void open();
	void open(const std::string &fileName);
	bool save();
	void save(const std::string &fileName, const std::string &sceneName);

	//void help();

private:
  DiagramView *myView;	
	ZLKeyBindings myBindings;

friend class ShowOptionsDialogAction;
friend class ShowInfoDialogAction;
friend class OpenSceneAction;
friend class SaveSceneAction;
friend class NewSceneAction;
friend class QuitAction;
};

#endif /* __GEOMETRICCALCULATOR_H__ */
