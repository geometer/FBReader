#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QWidget>
#include <QtGui/QLayout>

#include "ZLQtDialogContent.h"
#include "ZLQtOptionView.h"

void ZLQtDialogContent::close() {
    // calls before showing content, so,
    // for instance, here can be addStretch
}

ZLQtDialogContent::ZLQtDialogContent(QWidget *widget, const ZLResource &resource) : ZLDialogContent(resource), myWidget(widget) {
        myLayout = new QVBoxLayout;
        myLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        myWidget->setLayout(myLayout);
        isFirstWidget = true;
}

ZLQtDialogContent::~ZLQtDialogContent() {
}

void ZLQtDialogContent::addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) {
	createViewByEntry(name, tooltip, option);
}

void ZLQtDialogContent::addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0, const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1) {
	createViewByEntry(name0, tooltip0, option0);
	createViewByEntry(name1, tooltip1, option1);
}

void ZLQtDialogContent::addItem(QWidget *widget) {
        if (isFirstWidget == true) {
            myWidget->setFocusProxy(widget);
            isFirstWidget = false;
        }
        myLayout->addWidget(widget);

        //Expanding
        widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

void ZLQtDialogContent::createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) {
	if (option == 0) {
		return;
	}
	ZLQtOptionView *view = 0;
	switch (option->kind()) {
		case ZLOptionEntry::BOOLEAN:
			view = new BooleanOptionView(name, tooltip, (ZLBooleanOptionEntry*)option, this);
			break;
		case ZLOptionEntry::BOOLEAN3:
			view = new Boolean3OptionView(name, tooltip, (ZLBoolean3OptionEntry*)option, this);
			break;
		case ZLOptionEntry::STRING:
			view = new StringOptionView(name, tooltip, (ZLStringOptionEntry*)option, this, false);
			break;
		case ZLOptionEntry::PASSWORD:
			view = new StringOptionView(name, tooltip, (ZLStringOptionEntry*)option, this, true);
			break;
		case ZLOptionEntry::CHOICE:
			view = new ChoiceOptionView(name, tooltip, (ZLChoiceOptionEntry*)option, this);
			break;
		case ZLOptionEntry::SPIN:
			view = new SpinOptionView(name, tooltip, (ZLSpinOptionEntry*)option, this);
			break;
		case ZLOptionEntry::COMBO:
			view = new ComboOptionView(name, tooltip, (ZLComboOptionEntry*)option, this);
			break;
		case ZLOptionEntry::COLOR:
			view = new ColorOptionView(name, tooltip, (ZLColorOptionEntry*)option, this);
			break;
		case ZLOptionEntry::KEY:
			view = new KeyOptionView(name, tooltip, (ZLKeyOptionEntry*)option, this);
			break;
		case ZLOptionEntry::ORDER:
			//view = new OrderOptionView(name, tooltip, (ZLOrderOptionEntry*)option, this);
			break;
		case ZLOptionEntry::STATIC:
			view = new StaticTextOptionView(name, tooltip, (ZLStaticTextOptionEntry*)option, this);
			break;
		case ZLOptionEntry::PATH:
			view = new PathOptionView(name, tooltip, (ZLPathOptionEntry*)option, this);
			break;
	}

	if (view != 0) {
		view->setVisible(option->isVisible());
		addView(view);
	}
}

QWidget *ZLQtDialogContent::widget() {
	return myWidget;
}
