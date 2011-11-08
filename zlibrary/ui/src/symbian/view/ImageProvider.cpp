#include <QtCore/QDebug>

#include "ImageProvider.h"
#include "ImageUtils.h"
#include "../menu/DrillDownMenu.h"

ImageProvider::ImageProvider() {
    myTransformer = new PixmapTransfomer(this);
    myTransformer->start();

    myEmptyPixmap = QPixmap(MenuItemParameters::getImageSize());
    myEmptyPixmap.fill(Qt::transparent);

    connect(this, SIGNAL(needImage(const ZLTreeTitledNode*)), myTransformer, SLOT(needImage(const ZLTreeTitledNode*)), Qt::QueuedConnection);
    connect(myTransformer, SIGNAL(imageIsReady(const ZLTreeTitledNode*,QPixmap)), this, SLOT(imageIsReady(const ZLTreeTitledNode*,QPixmap)));
}

QPixmap ImageProvider::getImageForNode(const ZLTreeTitledNode *titledNode) const {
    if (myCache.contains(titledNode)) {
        return myCache.value(titledNode);
    }
    emit needImage(titledNode);
    return myEmptyPixmap;

}

void ImageProvider::imageIsReady(const ZLTreeTitledNode* node, QPixmap pixmap) const {
    myCache[node] = pixmap.isNull() ? myEmptyPixmap : pixmap;
    emit cacheUpdated();
}

 PixmapTransfomer::PixmapTransfomer(QObject *parent) : QThread(parent) {
    connect(&myManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRequestFinished(QNetworkReply*)));
 }

void PixmapTransfomer::needImage(const ZLTreeTitledNode* titledNode) const {
    QString imageUrl = QString::fromStdString(titledNode->imageUrl());
    qDebug() << Q_FUNC_INFO << imageUrl;
    if (imageUrl.isEmpty()) {
        //emit imageIsReady(titledNode,getZLImage(titledNode->image()));

        imageUrl = QString::fromStdString(ZLTreeTitledNode::ZLIMAGE_SCHEME + ZLTreeTitledNode::SCHEME_POSTFIX) +
                   QString::number(reinterpret_cast<qptrdiff>(titledNode), 16);
    }

    QUrl url = QUrl::fromEncoded(imageUrl.toStdString().c_str());
    //FIXME looks like this qDebug() output helps to show FS pictures with incorrect path (maybe because of url.toString)
    //without it, there's a problem with showing pictures from FS
    qDebug() << "PixmapTransfomer::needImage" << titledNode->imageUrl().c_str() <<  url.toString();
    if (myCache.contains(url)) {
        emit imageIsReady(titledNode, myCache.value(url));
        return;
    }
    if (url.scheme() == QLatin1String(ZLTreeTitledNode::ZLIMAGE_SCHEME.c_str())) {
        if (titledNode->image().isNull()) {
            return;
        }
        myCache[url] = getZLImage(titledNode->image());
        emit imageIsReady(titledNode, myCache[url]);
        return;
    } else if (url.scheme() == QLatin1String(ZLTreeTitledNode::LOCALFILE_SCHEME.c_str())) {
        myCache[url] = getFSImage(url);
        emit imageIsReady(titledNode, myCache[url]);
        return;
    } else {
        getNetworkImage(titledNode, url);
        return;
    }

    return;
}

QPixmap PixmapTransfomer::getZLImage(shared_ptr<ZLImage> image) const {
    return ImageUtils::ZLImageToQPixmap(image, 0, MenuItemParameters::getImageSize());
}

QPixmap PixmapTransfomer::getFSImage(QUrl url) const {
    return ImageUtils::fileUrlToQPixmap(url, 0, MenuItemParameters::getImageSize());
}

void PixmapTransfomer::getNetworkImage(const ZLTreeTitledNode* node, QUrl url) const {
    if (!url.isValid()) {
        return;
    }
    QNetworkRequest request(url);
    myNetworkImageCache[url] = node;
    myManager.get(request);
    return;
}

void PixmapTransfomer::onRequestFinished(QNetworkReply* reply) {
    if (!myNetworkImageCache.contains(reply->url())) {
        return;
    }

    QPixmap pixmap;
    pixmap.loadFromData(reply->readAll());
    QSize imageSize =  MenuItemParameters::getImageSize();
    if (!pixmap.isNull()) {
        pixmap = ImageUtils::scaleAndCenterPixmap(pixmap, imageSize, true);
    }
    const ZLTreeTitledNode* node = myNetworkImageCache.value(reply->url());
    myNetworkImageCache.remove(reply->url());

    if (pixmap.isNull()) {
        return;
    }
    myCache[reply->url()] = pixmap;
    emit imageIsReady(node, pixmap);
}

