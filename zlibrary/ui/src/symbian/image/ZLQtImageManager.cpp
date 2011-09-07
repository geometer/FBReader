#include <QtGui/QPainter>

#include <ZLImage.h>

#include "ZLQtImageManager.h"

ZLQtImageData::ZLQtImageData() : myImage(0), myX(0), myY(0) {
}

ZLQtImageData::~ZLQtImageData() {
	if (myImage != 0) {
		delete myImage;
	}
}

unsigned int ZLQtImageData::width() const {
	return (myImage != 0) ? myImage->width() : 0;
}

unsigned int ZLQtImageData::height() const {
	return (myImage != 0) ? myImage->height() : 0;
}

void ZLQtImageData::init(unsigned int width, unsigned int height) {
	if (myImage != 0) {
		delete myImage;
	}
	myImage = new QImage(width, height, QImage::Format_RGB32);
}

void ZLQtImageData::setPosition(unsigned int x, unsigned int y) {
	myX = x;
	myY = y;
}

void ZLQtImageData::moveX(int delta) {
	myX += delta;
}

void ZLQtImageData::moveY(int delta) {
	myY += delta;
}

void ZLQtImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	myImage->setPixel(myX, myY, qRgb(r, g, b));
}

void ZLQtImageData::copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY) {
	const QImage *sourceImage = ((const ZLQtImageData*)&source)->image();
	if ((myImage != 0) && (sourceImage != 0)) {
		QPainter painter(myImage);
		painter.drawImage(targetX, targetY, *sourceImage);
	}
}

shared_ptr<ZLImageData> ZLQtImageManager::createData() const {
	return new ZLQtImageData();
}

bool ZLQtImageManager::convertImageDirect(const std::string &stringData, ZLImageData &data) const {
	((ZLQtImageData&)data).init(0, 0);
	return ((ZLQtImageData&)data).myImage->loadFromData((const unsigned char*)stringData.data(), stringData.length());
}
