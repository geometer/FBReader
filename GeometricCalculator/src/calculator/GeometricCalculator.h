#ifndef __GEOMETRICCALCULATOR_H__
#define __GEOMETRICCALCULATOR_H__

#include <vector>

#include <shared_ptr.h>
#include <ZLView.h>
#include <ZLApplication.h>
#include <ZLKeyBindings.h>

class DiagramView;

class EditMode;

class GeometricCalculator : public ZLApplication { 

private:
	class UndoAction : public Action {

	public:
		UndoAction(DiagramView &view);
		bool isEnabled();
		void run();

	private:
		DiagramView &myView;
	};

	class RedoAction : public Action {

	public:
		RedoAction(DiagramView &view);
		bool isEnabled();
		void run();

	private:
		DiagramView &myView;
	};

	class SetModeAction : public Action {

	public:
		SetModeAction(DiagramView &view, shared_ptr<EditMode> editMode);
		bool isEnabled();
		void run();

	private:
		DiagramView &myView;
		shared_ptr<EditMode> myEditMode;
	};

	class OpenSceneAction : public Action {

	public:
		OpenSceneAction(GeometricCalculator &window);
		void run();

	private:
		GeometricCalculator &myWindow;
	};

	class NewSceneAction : public Action {

	public:
		NewSceneAction(GeometricCalculator &window);
		bool isEnabled();
		void run();

	private:
		GeometricCalculator &myWindow;
	};

public:
	GeometricCalculator(const std::string &fileName);
	~GeometricCalculator();

	void initWindow();
	ZLKeyBindings &keyBindings();

private:
	const std::string defaultSceneFileName() const;
	void createMenu();

	bool saveIfChanged();
	bool save(const char *fileName);

private:
	void newScene();
	void open();
	void open(const std::string &fileName);
	bool save();
	bool saveAs();

	void setMode();

	void help();

	void showFilePopup();

private:
  DiagramView *myView;	
	ZLKeyBindings myBindings;

friend class ShowOptionsDialogAction;
friend class ShowInfoDialogAction;
friend class OpenSceneAction;
friend class NewSceneAction;
friend class QuitAction;
};

#endif /* __GEOMETRICCALCULATOR_H__ */
