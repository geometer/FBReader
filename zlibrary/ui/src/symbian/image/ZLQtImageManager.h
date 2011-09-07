#ifndef __ZLQTIMAGEMANAGER_H__
#define __ZLQTIMAGEMANAGER_H__

#include <map>
#include <QtGui/QImage>

#include <ZLImageManager.h>

class ZLImage;

class ZLQtImageData : public ZLImageData {

public:
	ZLQtImageData();
	~ZLQtImageData();

	unsigned int width() const;
	unsigned int height() const;

	void init(unsigned int width, unsigned int height);
	void setPosition(unsigned int x, unsigned int y);
	void moveX(int delta);
	void moveY(int delta);
	void setPixel(unsigned char r, unsigned char g, unsigned char b);

	void copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY);

	const QImage *image() const { return myImage; }

private:
	QImage *myImage;
	unsigned int myX, myY;

friend class ZLQtImageManager;
};

class ZLQtImageManager : public ZLImageManager {

public:
	static void createInstance() { ourInstance = new ZLQtImageManager(); }

private:
	ZLQtImageManager() {}
	
protected:
	~ZLQtImageManager() {}
	shared_ptr<ZLImageData> createData() const;
	bool convertImageDirect(const std::string &stringData, ZLImageData &imageData) const;
};

#endif /* __ZLQTIMAGEMANAGER_H__ */
