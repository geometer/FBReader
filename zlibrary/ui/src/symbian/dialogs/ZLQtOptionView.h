#ifndef __ZLQTOPTIONVIEW_H__
#define __ZLQTOPTIONVIEW_H__

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtGui/QLabel>

#include <ZLFile.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include "../../../../core/src/dialogs/ZLOptionView.h"
#include <ZLDialogManager.h>

class ZLQtDialogContent;

class QLabel;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QGroupBox;
class QRadioButton;
class QComboBox;
class QSlider;
class QWidget;
class QGridLayout;

class QListWidget;
class QPushButton;

class ZLQtOptionView : public ZLOptionView {

protected:
	ZLQtOptionView(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, ZLQtDialogContent *tab) : ZLOptionView(name, tooltip, option), myTab(tab) {}

	void _show();
	void _hide();

        std::string removeShortcut(const std::string& name) const;

protected:
	ZLQtDialogContent *myTab;
	std::vector<QWidget*> myWidgets;
};

class ChoiceOptionView : public ZLQtOptionView {

public:
	ChoiceOptionView(const std::string &name, const std::string &tooltip, ZLChoiceOptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab), myButtons(0) {}
	~ChoiceOptionView() { if (myButtons != 0) delete[] myButtons; }

protected:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;

private:
	QGroupBox *myGroupBox;
	QRadioButton **myButtons;
};



class PressLabel : public QLabel {
	Q_OBJECT
public:
	PressLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0) : QLabel(text,parent,f) { }
	void mousePressEvent(QMouseEvent *ev) {
		emit labelPressed();
		QLabel::mousePressEvent(ev);
	}
signals:
	void labelPressed();
};

class BooleanOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	BooleanOptionView(const std::string &name, const std::string &tooltip, ZLBooleanOptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab) {}

protected:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;

private Q_SLOTS:
	void onStateChanged(bool) const;
	void onLabelPressed();

private:
	QCheckBox *myCheckBox;
	PressLabel* myLabel;
};

class Boolean3OptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	Boolean3OptionView(const std::string &name, const std::string &tooltip, ZLBoolean3OptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab) {}

protected:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;

private Q_SLOTS:
	void onStateChanged(int) const;

private:
	QCheckBox *myCheckBox;
};

class StringOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	StringOptionView(const std::string &name, const std::string &tooltip, ZLStringOptionEntry *option, ZLQtDialogContent *tab, bool passwordMode) : ZLQtOptionView(name, tooltip, option, tab), myLineEdit(0), myPasswordMode(passwordMode) {}

private:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();

private Q_SLOTS:
	void onValueEdited(const QString &value);

private:
	QLineEdit *myLineEdit;
	const bool myPasswordMode;
};



class PathOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT



public:
	PathOptionView(const std::string &name, const std::string &tooltip, ZLPathOptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab),
		myDirsListWidget(0), myAddButton(0), myDeleteButton(0) {}

private:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();
	void update();

private Q_SLOTS:
	void onAddButton();
	void onDeleteButton();
	void onItemActivated();

private:
	class SimpleFilter : public ZLOpenFileDialog::Filter {

	private:
		bool accepts(const ZLFile &file) const {
			return true;
		}
	};

private:
	QStringList myDirsList;
	QListWidget* myDirsListWidget;
	QPushButton* myAddButton;
	QPushButton* myDeleteButton;
};




class SpinOptionView : public ZLQtOptionView {

public:
	SpinOptionView(const std::string &name, const std::string &tooltip, ZLSpinOptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab), mySpinBox(0) {}

protected:
	void _createItem();
	void _onAccept() const;

private:
	QSpinBox *mySpinBox;
};

class ComboOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	ComboOptionView(const std::string &name, const std::string &tooltip, ZLComboOptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab), myComboBox(0) {}

private:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();

private Q_SLOTS:
	void onValueSelected(int index);
	void onValueEdited(const QString &value);
	
private:
	QComboBox *myComboBox;
};

class KeyOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	KeyOptionView(const std::string &name, const std::string &tooltip, ZLKeyOptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab), myComboBox(0), myKeyEditor(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;
	void reset();

private Q_SLOTS:
	void onValueChanged(int);

private:
	QComboBox *myComboBox;
	QLineEdit *myKeyEditor;
	std::string myCurrentKey;

friend class KeyLineEdit;
};

class ColorOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	ColorOptionView(const std::string &name, const std::string &tooltip, ZLColorOptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab), myRSlider(0), myGSlider(0), myBSlider(0), myColorBar(0) {}

private:
	void _createItem();
	void _onAccept() const;
	void reset();

	QSlider *createColorSlider(QGridLayout *layout, int index, const ZLResource &resource, int value);

private Q_SLOTS:
	void onSliderMove(int);

private:
	QSlider *myRSlider, *myGSlider, *myBSlider;
	QLabel *myColorBar;
};

class StaticTextOptionView : public ZLQtOptionView {

public:
	StaticTextOptionView(const std::string &name, const std::string &tooltip, ZLStaticTextOptionEntry *option, ZLQtDialogContent *tab) : ZLQtOptionView(name, tooltip, option, tab), myLabel(0) {}

private:
	void _createItem();
	void _onAccept() const;

private:
	QLabel *myLabel;
};

class PictureView : public ZLQtOptionView {

public:
        PictureView(const std::string &name, const std::string &tooltip, ZLPictureOptionEntry *option, ZLQtDialogContent *tab);

private:
        void _createItem();
        void _onAccept() const;

private:
        shared_ptr<ZLImage> myImage;
};

class PictureWidget : public QWidget {
public:
    PictureWidget(QPixmap picture, QWidget* parent = 0);
    QSize sizeHint () const;

protected:
    //TODO implement caching of image and ResizeEvent
    //void resizeEvent ( QResizeEvent * event )
    void paintEvent(QPaintEvent *event);

protected:
    /*const*/ QPixmap myPicture;
    QPixmap myCachePicture;
};

#endif /* __ZLQTOPTIONVIEW_H__ */
