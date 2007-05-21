#include <ZLPaintContext.h>

#include "SampleApplication.h"

SampleApplicationView::SampleApplicationView(ZLApplication &application, ZLPaintContext &context) : ZLView(application, context), myZoom(100), myMode(GEOMETRIC) {
}

const std::string &SampleApplicationView::caption() const {
	static const std::string _caption = "Sample";
	return _caption;
}

void SampleApplicationView::paint() {
	switch (myMode) {
		case GEOMETRIC:
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
			break;
		case TEXT:
			context().clear(ZLColor(191, 255, 255));
			context().setColor(ZLColor(0, 0, 255));
			std::string str = "HELLO";
			std::string str1 = "ПРИВЕТ";
			int y = myZoom / 10;
			context().setFont("Arial", myZoom / 5, false, false);
			y += context().stringHeight();
			context().drawString(myZoom / 10, y, str.data(), str.length());
			y += context().stringHeight();
			context().drawString(myZoom / 10, y, str1.data(), str1.length());
			context().setFont("Georgia", myZoom / 5, false, false);
			y += context().stringHeight();
			context().drawString(myZoom / 10, y, str.data(), str.length());
			y += context().stringHeight();
			context().drawString(myZoom / 10, y, str1.data(), str1.length());
			break;
	}
}

SampleApplication::SampleApplication() : ZLApplication("SampleApplication"), myBindings("Keys") {
	SampleApplicationView *view = new SampleApplicationView(*this, context());

	addAction(1, new ZoomAction(*view, true));
	addAction(2, new ZoomAction(*view, false));
	addAction(3, new SetModeAction(*view, SampleApplicationView::GEOMETRIC));
	addAction(4, new SetModeAction(*view, SampleApplicationView::TEXT));

	shared_ptr<Toolbar::ButtonGroup> group = toolbar().createButtonGroup(3);
	toolbar().addButton(3, ZLResourceKey("geo"), group);
	toolbar().addButton(4, ZLResourceKey("alpha"), group);
	toolbar().addSeparator();
	toolbar().addButton(1, ZLResourceKey("plus"));
	toolbar().addButton(2, ZLResourceKey("minus"));

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

SetModeAction::SetModeAction(SampleApplicationView &view, SampleApplicationView::Mode mode) : myView(view), myMode(mode) {
}

void SetModeAction::run() {
	myView.myMode = myMode;
	myView.repaintView();
}
