/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <algorithm>

#include <QtGui/QLayout>
#include <QtGui/QScrollBar>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QTapGesture>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtCore/QElapsedTimer>
#include <QtCore/QDebug>
#include <QtOpenGL/QGLWidget>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include "../dialogs/ZLQmlDialogManager.h"
#include "ZLQmlSwipeGestureRecognizer.h"

#include <ZLibrary.h>
#include <ZLLanguageUtil.h>

#include "ZLQmlViewWidget.h"
#include "ZLQmlPaintContext.h"

class MyQScrollBar : public QScrollBar {

public:
	MyQScrollBar(Qt::Orientation orientation, QWidget *parent) : QScrollBar(orientation, parent) {
	}

private:
	void mouseMoveEvent(QMouseEvent *event) {
		if (orientation() == Qt::Vertical) {
			const int y = event->y();
			if ((y <= 0) || (y >= height())) {
				return;
			}
		} else {
			const int x = event->x();
			if ((x <= 0) || (x >= width())) {
				return;
			}
		}
		QScrollBar::mouseMoveEvent(event);
	}
};

void ZLQmlViewObject::repaint()	{
	qDebug() << Q_FUNC_INFO;
	if (myContent)
		myContent->update();
}

void ZLQmlViewObject::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
//	qDebug() << Q_FUNC_INFO << direction << enabled;
	if (direction == ZLView::VERTICAL)
		myVerticalInfo->setEnabled(enabled);
	else
		myHorizontalInfo->setEnabled(enabled);
//	if (direction == ZLView::VERTICAL) {
//		myRightScrollBar->setShown(enabled && myShowScrollBarAtRight);
//		myLeftScrollBar->setShown(enabled && !myShowScrollBarAtRight);
//	} else {
//		myBottomScrollBar->setShown(enabled && myShowScrollBarAtBottom);
//		myTopScrollBar->setShown(enabled && !myShowScrollBarAtBottom);
//	}
}

void ZLQmlViewObject::setScrollbarPlacement(ZLView::Direction direction, bool standard) {
//	if ((rotation() == ZLView::DEGREES90) || (rotation() == ZLView::DEGREES270)) {
//		if (ZLLanguageUtil::isRTLLanguage(ZLibrary::Language())) {
//			standard = !standard;
//		}
//	}
//	if (direction == ZLView::VERTICAL) {
//		if (standard != myShowScrollBarAtRight) {
//			myShowScrollBarAtRight = standard;
//			QScrollBar *old = standard ? myLeftScrollBar : myRightScrollBar;
//			QScrollBar *current = standard ? myRightScrollBar : myLeftScrollBar;
//			if (old->isVisible()) {
//				old->hide();
//				current->show();
//			}
//		}
//	} else {
//		if (standard != myShowScrollBarAtBottom) {
//			myShowScrollBarAtBottom = standard;
//			QScrollBar *old = standard ? myTopScrollBar : myBottomScrollBar;
//			QScrollBar *current = standard ? myBottomScrollBar : myTopScrollBar;
//			if (old->isVisible()) {
//				old->hide();
//				current->show();
//			}
//		}
//	}
}

void ZLQmlViewObject::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to) {
//	qDebug() << Q_FUNC_INFO << direction << full << from << to;
	if (direction == ZLView::VERTICAL)
		myVerticalInfo->setParameters(full, from, to);
	else
		myHorizontalInfo->setParameters(full, from, to);
//	if (direction == ZLView::VERTICAL) {
//		myContent->setHeight(full + from - to);
//	} else {
//		myContent->setWidth(full + from - to);
//	}
//	QScrollBar *bar =
//		(direction == ZLView::VERTICAL) ?
//			(myShowScrollBarAtRight ? myRightScrollBar : myLeftScrollBar) :
//			(myShowScrollBarAtBottom ? myBottomScrollBar : myTopScrollBar);
//	bar->setMinimum(0);
//	bar->setMaximum(full + from - to);
//	bar->setValue(from);
//	bar->setPageStep(to - from);
}

void ZLQmlViewObject::onVerticalSliderMoved(int value) {
//	QScrollBar *bar =
//		myShowScrollBarAtRight ? myRightScrollBar : myLeftScrollBar;
//	int maxValue = bar->maximum();
//	int pageStep = bar->pageStep();
//	value = std::max(std::min(value, maxValue), 0);
//	onScrollbarMoved(
//		ZLView::VERTICAL,
//		maxValue + pageStep,
//		value,
//		value + pageStep
//	);
}

void ZLQmlViewObject::onHorizontalSliderMoved(int value) {
//	QScrollBar *bar =
//		myShowScrollBarAtBottom ? myBottomScrollBar : myTopScrollBar;
//	int maxValue = bar->maximum();
//	int pageStep = bar->pageStep();
//	value = std::max(std::min(value, maxValue), 0);
//	onScrollbarMoved(
//		ZLView::HORIZONTAL,
//		maxValue + pageStep,
//		value,
//		value + pageStep
//	);
}

void ZLQmlViewObject::onVerticalSliderClicked(int value) {
//	switch (value) {
//		case QScrollBar::SliderSingleStepAdd:
//			onScrollbarStep(ZLView::VERTICAL, 1);
//			break;
//		case QScrollBar::SliderSingleStepSub:
//			onScrollbarStep(ZLView::VERTICAL, -1);
//			break;
//		case QScrollBar::SliderPageStepAdd:
//			onScrollbarPageStep(ZLView::VERTICAL, 1);
//			break;
//		case QScrollBar::SliderPageStepSub:
//			onScrollbarPageStep(ZLView::VERTICAL, -1);
//			break;
//	}
}

void ZLQmlViewObject::onHorizontalSliderClicked(int value) {
//	switch (value) {
//		case QScrollBar::SliderSingleStepAdd:
//			onScrollbarStep(ZLView::HORIZONTAL, 1);
//			break;
//		case QScrollBar::SliderSingleStepSub:
//			onScrollbarStep(ZLView::HORIZONTAL, -1);
//			break;
//		case QScrollBar::SliderPageStepAdd:
//			onScrollbarPageStep(ZLView::HORIZONTAL, 1);
//			break;
//		case QScrollBar::SliderPageStepSub:
//			onScrollbarPageStep(ZLView::HORIZONTAL, -1);
//			break;
//	}
}

QWidget *ZLQmlViewObject::widget() {
	return myWidget;
}

QDeclarativeItem *ZLQmlViewObject::bookView() const {
	return myContent;
}

void ZLQmlViewObject::setBookView(QDeclarativeItem *bookView) {
	if (myContent == bookView)
		return;
	myContent = bookView;
	emit bookViewChanged(myContent);
}

QObject *ZLQmlViewObject::verticalScrollBar() const {
	return myVerticalInfo;
}

QObject *ZLQmlViewObject::horizontalScrollBar() const {
	return myHorizontalInfo;
}

void ZLQmlViewObject::handlePress(int x, int y) {
	view()->onStylusPress(x, y);
}

void ZLQmlViewObject::handleRelease(int x, int y) {
	view()->onStylusRelease(x, y);
}

void ZLQmlViewObject::handleMove(int x, int y) {
	view()->onStylusMove(x, y);
}

void ZLQmlViewObject::handleMovePressed(int x, int y) {
	view()->onStylusMovePressed(x, y);
}

void ZLQmlViewObject::handleFingerTap(int x, int y) {
	view()->onFingerTap(x, y);
}

void ZLQmlViewObject::handleScrollBarMoved(int full, int from, int to) {
	if (from < 0) {
		to -= from;
		from = 0;
	}
//	qDebug() << Q_FUNC_INFO << full << from << to;
	onScrollbarMoved(ZLView::VERTICAL, full, from, to);
//	repaint();
}

void ZLQmlViewObject::handleScrollBarStep(int steps) {
	onScrollbarStep(ZLView::VERTICAL, steps);
}

void ZLQmlViewObject::handleScrollBarPageStep(int steps) {
	onScrollbarPageStep(ZLView::VERTICAL, steps);
}

QScrollBar *ZLQmlViewObject::addScrollBar(QGridLayout *layout, Qt::Orientation orientation, int x, int y) {
//	QScrollBar *scrollBar = new MyQScrollBar(orientation, myFrame);
//	layout->addWidget(scrollBar, x, y);
//	scrollBar->hide();
//	if (orientation == Qt::Vertical) {
//		connect(scrollBar, SIGNAL(sliderMoved(int)), this, SLOT(onVerticalSliderMoved(int)));
//		connect(scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(onVerticalSliderClicked(int)));
//	} else {
//		connect(scrollBar, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSliderMoved(int)));
//		connect(scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(onHorizontalSliderClicked(int)));
//	}
//	return scrollBar;
	return 0;
}

ZLQmlViewObject::ZLQmlViewObject(QObject *parent, ZLApplication *application)
    : QObject(parent), ZLViewWidget((ZLView::Angle)application->AngleStateOption.value()), myApplication(application) {
//	myFrame = new QWidget();
//	QGridLayout *layout = new QGridLayout();
//	layout->setMargin(0);
//	layout->setSpacing(0);
//	myFrame->setLayout(layout);
	myContent = 0;
	myVerticalInfo = new ZLQmlScrollBarInfo(this);
	myHorizontalInfo = new ZLQmlScrollBarInfo(this);
	myWidget = new ZLQmlViewWidget(0, *this);
//	layout->addWidget(myWidget, 1, 1);

//	myRightScrollBar = addScrollBar(layout, Qt::Vertical, 1, 2);
//	myLeftScrollBar = addScrollBar(layout, Qt::Vertical, 1, 0);
//	myShowScrollBarAtRight = true;

//	myBottomScrollBar = addScrollBar(layout, Qt::Horizontal, 2, 1);
//	myTopScrollBar = addScrollBar(layout, Qt::Horizontal, 0, 1);
//	myShowScrollBarAtBottom = true;
}

void ZLQmlViewObject::trackStylus(bool track) {
//	qDebug("%s %s", Q_FUNC_INFO, track ? "true" : "false");
	myWidget->setMouseTracking(track);
}

ZLQmlScrollBarInfo::ZLQmlScrollBarInfo(QObject *parent) : myEnabled(false) {
	myHeight = myTop = myBottom = 0;
}

ZLQmlScrollBarInfo::~ZLQmlScrollBarInfo() {
}

void ZLQmlScrollBarInfo::setEnabled(bool enabled) {
	if (enabled == myEnabled)
		return;
	myEnabled = enabled;
	emit enabledChanged(myEnabled);
}

void ZLQmlScrollBarInfo::setParameters(int full, int from, int to) {
	if (full != myHeight) {
		myHeight = full;
		emit heightChanged(myHeight);
	}
	if (from != myTop) {
		myTop = from;
		emit topChanged(myTop);
	}
	if (to != myBottom) {
		myBottom = to;
		emit bottomChanged(myBottom);
	}
}

bool ZLQmlScrollBarInfo::enabled() const {
	return myEnabled;
}

int ZLQmlScrollBarInfo::height() const {
	return myHeight;
}

int ZLQmlScrollBarInfo::top() const {
	return myTop;
}

int ZLQmlScrollBarInfo::bottom() const {
	return myBottom;
}

ZLQmlViewWidget::ZLQmlViewWidget(QWidget *parent, ZLQmlViewObject &holder) : QDeclarativeView(parent), myHolder(holder) {
	//setBackgroundMode(NoBackground);
	QFont font;
	font.setFamily(QLatin1String("Nokia Pure"));
	font.setPointSize(24);
	qApp->setFont(font);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	rootContext()->setContextProperty(QLatin1String("applicationInfo"), holder.parent());
	rootContext()->setContextProperty(QLatin1String("objectHolder"), &holder);
	ZLDialogManager *dialogManager = &ZLDialogManager::Instance();
	QObject *qDialogManager = static_cast<ZLQmlDialogManager*>(dialogManager);
	rootContext()->setContextProperty(QLatin1String("dialogManager"), qDialogManager);
	setViewport(new QGLWidget(this));
	
	// TODO: Find the way to get this path through API
	setSource(QUrl::fromLocalFile("/usr/share/zlibrary/declarative/Main.qml"));
}

//void ZLQmlViewWidget::paintEvent(QPaintEvent*) {
//	ZLQtPaintContext &context = (ZLQtPaintContext&)myHolder.view()->context();
//	switch (myHolder.rotation()) {
//		default:
//			context.setSize(width(), height());
//			break;
//		case ZLView::DEGREES90:
//		case ZLView::DEGREES270:
//			context.setSize(height(), width());
//			break;
//	}
//	myHolder.view()->paint();
//	QPainter realPainter(this);
//	switch (myHolder.rotation()) {
//		default:
//			realPainter.drawPixmap(0, 0, context.pixmap());
//			break;
//		case ZLView::DEGREES90:
//			realPainter.rotate(270);
//			realPainter.drawPixmap(1 - height(), -1, context.pixmap());
//			break;
//		case ZLView::DEGREES180:
//			realPainter.rotate(180);
//			realPainter.drawPixmap(1 - width(), 1 - height(), context.pixmap());
//			break;
//		case ZLView::DEGREES270:
//			realPainter.rotate(90);
//			realPainter.drawPixmap(-1, 1 - width(), context.pixmap());
//			break;
//	}
//}

//void ZLQmlViewWidget::mousePressEvent(QMouseEvent *event) {
//	myHolder.view()->onStylusMove(x(event), y(event));
//	myHolder.view()->onStylusPress(x(event), y(event));
//}

//void ZLQmlViewWidget::mouseReleaseEvent(QMouseEvent *event) {
//	myHolder.view()->onStylusRelease(x(event), y(event));
//}

//void ZLQmlViewWidget::mouseMoveEvent(QMouseEvent *event) {
//	switch (event->buttons()) {
//		case Qt::LeftButton:
//			myHolder.view()->onStylusMovePressed(x(event), y(event));
//			break;
//		case Qt::NoButton:
//			myHolder.view()->onStylusMove(x(event), y(event));
//			break;
//		default:
//			break;
//	}
//}

//int ZLQmlViewWidget::x(const QMouseEvent *event) const {
//	const int maxX = width() - 1;
//	const int maxY = height() - 1;
//	switch (myHolder.rotation()) {
//		default:
//			return std::min(std::max(event->x(), 0), maxX);
//		case ZLView::DEGREES90:
//			return maxY - std::min(std::max(event->y(), 0), maxY);
//		case ZLView::DEGREES180:
//			return maxX - std::min(std::max(event->x(), 0), maxX);
//		case ZLView::DEGREES270:
//			return std::min(std::max(event->y(), 0), maxY);
//	}
//}

//int ZLQmlViewWidget::y(const QMouseEvent *event) const {
//	const int maxX = width() - 1;
//	const int maxY = height() - 1;
//	switch (myHolder.rotation()) {
//		default:
//			return std::min(std::max(event->y(), 0), maxY);
//		case ZLView::DEGREES90:
//			return std::min(std::max(event->x(), 0), maxX);
//		case ZLView::DEGREES180:
//			return maxY - std::min(std::max(event->y(), 0), maxY);
//		case ZLView::DEGREES270:
//			return maxX - std::min(std::max(event->x(), 0), maxX);
//	}
//}

ZLQmlBookContent::ZLQmlBookContent(QDeclarativeItem *parent) : QDeclarativeItem(parent), myHolder(0) {
	setFlag(ItemHasNoContents, false);
//	setFlag(ItemUsesExtendedStyleOption, false);
//	setAcceptedMouseButtons(Qt::LeftButton);
//	setAcceptTouchEvents(true);
//	setFiltersChildEvents(true);
//	grabGesture(Qt::TapGesture, Qt::IgnoredGesturesPropagateToParent);
//	grabGesture(ZLQmlSwipeGestureRecognizer::gestureType());
	myVisibleHeight = 400;
}

ZLQmlBookContent::~ZLQmlBookContent() {
}

bool ZLQmlBookContent::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::Gesture) {;
		QGestureEvent *gestureEvent = static_cast<QGestureEvent*>(event);
		qDebug() << Q_FUNC_INFO << obj << gestureEvent->gestures();
	}
	return QDeclarativeItem::eventFilter(obj, event);
}

ZLQmlViewObject *ZLQmlBookContent::objectHolder() const {
	return myHolder;
}

void ZLQmlBookContent::setObjectHolder(QObject *objectHolder) {
	if (objectHolder == myHolder)
		return;
	myHolder = qobject_cast<ZLQmlViewObject*>(objectHolder);
	emit objectHolderChanged(myHolder);
}

int ZLQmlBookContent::visibleHeight() const {
	return myVisibleHeight;
}

void ZLQmlBookContent::setVisibleHeight(int visibleHeight) {
	if (myVisibleHeight == visibleHeight)
		return;
	myVisibleHeight = visibleHeight;
	emit visibleHeightChanged(myVisibleHeight);
}

void ZLQmlBookContent::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	if (!myHolder) {
		qDebug("%s: Can't find objectHolder", Q_FUNC_INFO);
		return;
	}
	QElapsedTimer timer;
	timer.start();
//	painter->save();
//	painter->translate(0, myVisibleHeight);
	ZLQmlPaintContext &context = static_cast<ZLQmlPaintContext&>(myHolder->view()->context());
	context.beginPaint(width(), height(), painter);
	myHolder->view()->paint();
	context.endPaint();
//	painter->translate(0, -myVisibleHeight);
//	painter->restore();
//	qDebug("%s %lld ms", Q_FUNC_INFO, timer.elapsed());
}

bool ZLQmlBookContent::sceneEvent(QEvent *event) {
//	if (event->type() == QEvent::Gesture && isVisibleTo(0)) {
//		QGestureEvent *gestureEvent = static_cast<QGestureEvent*>(event);
//		qDebug() << Q_FUNC_INFO << gestureEvent->gestures();
//		gestureEvent->ignore();
//		if (QGesture *gesture = gestureEvent->gesture(Qt::TapGesture)) {
////			gestureEvent->accept(Qt::TapGesture);
//			emit tap(gesture);
//		}
//		if (QGesture *gesture = gestureEvent->gesture(Qt::SwipeGesture)) {
////			gestureEvent->accept(Qt::SwipeGesture);
//			if (gesture->state() == Qt::GestureFinished)
//				emit swipe(gesture);
//		}
//	}
	return QDeclarativeItem::sceneEvent(event);
}

//void ZLQmlBookContent::mousePressEvent(QGraphicsSceneMouseEvent *event) {
//	if (!myHolder)
//		return;
////	qDebug("%s (%d, %d)", Q_FUNC_INFO, int(event->pos().x()), int(event->pos().y()));
////	myMousePos = event->pos();
//	myHolder->view()->onStylusMove(event->pos().x(), event->pos().y());
//	myHolder->view()->onStylusPress(event->pos().x(), event->pos().y());
//}

//void ZLQmlBookContent::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
//	if (!myHolder)
//		return;
////	qDebug("%s (%d, %d)", Q_FUNC_INFO, int(event->pos().x()), int(event->pos().y()));
//	if (event->buttons() & Qt::LeftButton)
//		myHolder->view()->onStylusMovePressed(event->pos().x(), event->pos().y());
//	else
//		myHolder->view()->onStylusMove(event->pos().x(), event->pos().y());
//}

//void ZLQmlBookContent::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
//	if (!myHolder)
//		return;
////	qDebug("%s (%d, %d)", Q_FUNC_INFO, int(event->pos().x()), int(event->pos().y()));
//	myHolder->view()->onStylusRelease(event->pos().x(), event->pos().y());
//}
