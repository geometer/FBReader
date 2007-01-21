#include <ZLPaintContext.h>

#include "SampleApplication.h"

SampleApplicationView::SampleApplicationView(ZLApplication &application, ZLPaintContext &context) : ZLView(application, context), myZoom(100) {
}

const std::string &SampleApplicationView::caption() const {
	static const std::string _caption = "Sample";
	return _caption;
}

void SampleApplicationView::paint() {
	context().clear(ZLColor(255, 255, 127));
	context().setColor(ZLColor(0, 0, 255));
	context().drawLine(myZoom, myZoom, myZoom, 4 * myZoom);
	context().drawLine(5 * myZoom / 2, myZoom, 5 * myZoom / 2, 4 * myZoom);
	context().drawLine(4 * myZoom, myZoom, 4 * myZoom, 4 * myZoom);
	context().drawLine(myZoom, myZoom, 4 * myZoom, myZoom);
	context().drawLine(myZoom, 5 * myZoom / 2, 4 * myZoom, 5 * myZoom / 2);
	context().drawLine(myZoom, 4 * myZoom, 4 * myZoom, 4 * myZoom);
	context().setFillColor(ZLColor(127, 127, 127));
	context().fillRectangle(myZoom + 1, myZoom + 1, 5 * myZoom / 2 - 1, 5 * myZoom / 2 - 1);
	context().setFillColor(ZLColor(0, 255, 0));
	context().fillRectangle(myZoom + 1, 5 * myZoom / 2 + 1, 5 * myZoom / 2 - 1, 4 * myZoom - 1);
	context().setFillColor(ZLColor(127, 127, 255));
	context().fillRectangle(5 * myZoom / 2 + 1, myZoom + 1, 4 * myZoom - 1, 5 * myZoom / 2 - 1);
	context().setFillColor(ZLColor(127, 0, 0));
	context().fillRectangle(5 * myZoom / 2 + 1, 5 * myZoom / 2 + 1, 4 * myZoom - 1, 4 * myZoom - 1);
	context().setColor(ZLColor(0, 0, 0), ZLPaintContext::DASH_LINE);
	context().setFillColor(ZLColor(255, 255, 0), ZLPaintContext::HALF_FILL);
	context().drawFilledCircle(5 * myZoom / 2, 5 * myZoom / 2, 3 * myZoom / 4);
}

SampleApplication::SampleApplication() : ZLApplication("SampleApplication"), myBindings("Keys") {
	SampleApplicationView *view = new SampleApplicationView(*this, context());

	addAction(1, new ZoomAction(*view, true));
	addAction(2, new ZoomAction(*view, false));

	toolbar().addButton(1, "plus");
	toolbar().addButton(2, "minus");

	setView(view);
}

SampleApplication::~SampleApplication() {
}

ZLKeyBindings &SampleApplication::keyBindings() {
	return myBindings;
}

ZoomAction::ZoomAction(SampleApplicationView &view, bool increase) : myView(view), myIncrease(increase) {
}

void ZoomAction::run() {
	if (myIncrease) {
		myView.myZoom += 10;
	} else {
		myView.myZoom -= 10;
	}
	myView.repaintView();
}

bool ZoomAction::isEnabled() {
	if (myIncrease) {
		return myView.myZoom < 150;
	} else {
		return myView.myZoom > 50;
	}
}
