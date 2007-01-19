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
};

class SampleApplication : public ZLApplication { 

public:
	SampleApplication();
	~SampleApplication();

	ZLKeyBindings &keyBindings();

private:
	ZLKeyBindings myBindings;
};

#endif /* __SAMPLEAPPLICATION_H__ */
