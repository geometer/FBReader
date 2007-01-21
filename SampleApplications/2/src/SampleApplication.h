#ifndef __SAMPLEAPPLICATION_H__
#define __SAMPLEAPPLICATION_H__

#include <ZLView.h>
#include <ZLApplication.h>
#include <ZLKeyBindings.h>

class SampleApplicationView : public ZLView {

public:
	SampleApplicationView(ZLApplication &application, ZLPaintContext &context);

	const std::string &caption() const;
	void paint();

private:
	int myZoom;

friend class ZoomAction;
};

class SampleApplication : public ZLApplication { 

public:
	SampleApplication();
	~SampleApplication();

	ZLKeyBindings &keyBindings();

private:
	ZLKeyBindings myBindings;
};

class ZoomAction : public ZLApplication::Action {

public:
	ZoomAction(SampleApplicationView &view, bool increase);

	void run();
	bool isEnabled();

private:
	SampleApplicationView &myView;
	const bool myIncrease;
};

#endif /* __SAMPLEAPPLICATION_H__ */
