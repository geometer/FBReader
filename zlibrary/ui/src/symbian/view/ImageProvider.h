#ifndef __IMAGEPROVIDER_H__
#define __IMAGEPROVIDER_H__

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>
#include <QtGui/QPixmap>

#include <ZLImageManager.h>

class ImageProvider : public QObject {
    Q_OBJECT

public:
    static ImageProvider& Instance();
    //TODO implement deleteInstance

public:
    QPixmap getUrlImage(QUrl url) const;
    QPixmap getFromZLImage(shared_ptr<ZLImage> image) const;
    QPixmap getFromZLImage(QString cacheId, shared_ptr<ZLImage> image) const;

signals:
//    void imageTransfromed();
//    void imageLoaded();
    void cacheUpdated();

private:
    ImageProvider();

private slots:
        void onRequestFinished(QNetworkReply*);

private:
        const QPixmap& downloadImage(QUrl url) const;

private:
        mutable QNetworkAccessManager myManager;
        mutable QMap<QString,QPixmap> myCache;
        mutable QMap<QString,QPixmap> myZLImageCache;
        QPixmap myEmptyPixmap;
        //TODO cache should not be deleted after closing net library dialog (??)


private:
    static ImageProvider* myInstance;


};

#endif /* __IMAGEPROVIDER_H__ */

