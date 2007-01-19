#include <ZLPaintContext.h>

#include "SampleApplication.h"

SampleApplicationView::SampleApplicationView(ZLApplication &application, ZLPaintContext &context) : ZLView(application, context) {
}

const std::string &SampleApplicationView::caption() const {
	static const std::string _caption = "Sample";
	return _caption;
}

void SampleApplicationView::paint() {
	context().clear(ZLColor(255, 255, 255));
}

SampleApplication::SampleApplication() : ZLApplication("SampleApplication"), myBindings("Keys") {
	setView(new SampleApplicationView(*this, context()));
}

SampleApplication::~SampleApplication() {
}

ZLKeyBindings &SampleApplication::keyBindings() {
	return myBindings;
}
