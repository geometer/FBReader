#ifndef __ZLQTVIEWWIDGET_H__
#define __ZLQTVIEWWIDGET_H__

#include <QtGui/QWidget>

#include "../../../../core/src/view/ZLViewWidget.h"
#include <ZLApplication.h>

class QGridLayout;
class QScrollBar;

class ZLQtViewWidget : public QObject, public ZLViewWidget {
	Q_OBJECT

private:
	class Widget : public QWidget {

	public:
		Widget(QWidget *parent, ZLQtViewWidget &holder);

	private:
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

		int x(const QMouseEvent *event) const;
		int y(const QMouseEvent *event) const;

	private:
		ZLQtViewWidget &myHolder;
	};
	
public:
	ZLQtViewWidget(QWidget *parent, ZLApplication *application);
	QWidget *widget();

private Q_SLOTS:
	void onVerticalSliderMoved(int value);
	void onHorizontalSliderMoved(int value);
	void onVerticalSliderClicked(int value);
	void onHorizontalSliderClicked(int value);

private:
	void repaint();
	void trackStylus(bool track);

	void setScrollbarEnabled(ZLView::Direction direction, bool enabled);
	void setScrollbarPlacement(ZLView::Direction direction, bool standard);
	void setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to);

protected:
        virtual QScrollBar *addScrollBar(QGridLayout *layout, Qt::Orientation orientation, int x, int y);

protected: ////
	QWidget *myFrame;
	Widget *myQWidget;

	QScrollBar *myRightScrollBar;
	QScrollBar *myLeftScrollBar;
	bool myShowScrollBarAtRight;

	QScrollBar *myBottomScrollBar;
	QScrollBar *myTopScrollBar;
	bool myShowScrollBarAtBottom;

	ZLApplication *myApplication;
};

#endif /* __ZLQTVIEWWIDGET_H__ */
