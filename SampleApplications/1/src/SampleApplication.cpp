#include <ZLPaintContext.h>

#include "SampleApplication.h"

SampleApplicationView::SampleApplicationView(ZLApplication &application, ZLPaintContext &context) : ZLView(application, context) {
}

const std::string &SampleApplicationView::caption() const {
	static const std::string _caption = "Sample";
	return _caption;
}

void SampleApplicationView::paint() {
	context().clear(ZLColor(255, 255, 127));
	context().setColor(ZLColor(0, 0, 255));
	context().drawLine(100, 100, 100, 400);
	context().drawLine(250, 100, 250, 400);
	context().drawLine(400, 100, 400, 400);
	context().drawLine(100, 100, 400, 100);
	context().drawLine(100, 250, 400, 250);
	context().drawLine(100, 400, 400, 400);
	context().setFillColor(ZLColor(127, 127, 127));
	context().fillRectangle(101, 101, 249, 249);
	context().setFillColor(ZLColor(0, 255, 0));
	context().fillRectangle(101, 251, 249, 399);
	context().setFillColor(ZLColor(127, 127, 255));
	context().fillRectangle(251, 101, 399, 249);
	context().setFillColor(ZLColor(127, 0, 0));
	context().fillRectangle(251, 251, 399, 399);
	context().setColor(ZLColor(0, 0, 0), ZLPaintContext::DASH_LINE);
	context().setFillColor(ZLColor(255, 255, 0), ZLPaintContext::HALF_FILL);
	context().drawFilledCircle(250, 250, 75);
}

SampleApplication::SampleApplication() : ZLApplication("SampleApplication"), myBindings("Keys") {
	setView(new SampleApplicationView(*this, context()));
}

SampleApplication::~SampleApplication() {
}

ZLKeyBindings &SampleApplication::keyBindings() {
	return myBindings;
}
