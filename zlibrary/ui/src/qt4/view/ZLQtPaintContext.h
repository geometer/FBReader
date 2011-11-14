#ifndef __ZLQTPAINTCONTEXT_H__
#define __ZLQTPAINTCONTEXT_H__

#include <ZLPaintContext.h>

class QPainter;
class QPixmap;

class ZLQtPaintContext : public ZLPaintContext {

public:
	ZLQtPaintContext();
	~ZLQtPaintContext();

	const QPixmap &pixmap() const { return *myPixmap; }

	void setSize(int w, int h);

	int width() const;
	int height() const;

	void clear(ZLColor color);

	void fillFamiliesList(std::vector<std::string> &families) const;
	const std::string realFontFamilyName(std::string &fontFamily) const;

	void setFont(const std::string &family, int size, bool bold, bool italic);
	void setColor(ZLColor color, LineStyle style = SOLID_LINE);
	void setFillColor(ZLColor color, FillStyle style = SOLID_FILL);

	int stringWidth(const char *str, int len, bool rtl) const;
	int spaceWidth() const;
	int stringHeight() const;
	int descent() const;
	void drawString(int x, int y, const char *str, int len, bool rtl);

	void drawImage(int x, int y, const ZLImageData &image);
	void drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type);

	void drawLine(int x0, int y0, int x1, int y1);
	void fillRectangle(int x0, int y0, int x1, int y1);
	void drawFilledCircle(int x, int y, int r);

private:
	QPainter *myPainter;
	QPixmap *myPixmap;
	mutable int mySpaceWidth;
	int myDescent;

	bool myFontIsStored;
	std::string myStoredFamily;
	int myStoredSize;
	bool myStoredBold;
	bool myStoredItalic;	
};

#endif /* __ZLQTPAINTCONTEXT_H__ */
