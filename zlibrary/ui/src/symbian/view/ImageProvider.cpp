#include <QtCore/QDebug>
#include <QtCore/QThreadPool>

#include "ImageProvider.h"
#include "ImageUtils.h"
#include "../menu/DrillDownMenu.h"

ImageRunnable::ImageRunnable(const ZLTreeTitledNode *node, QObject *requester) : myRequester(requester), myNode(node) {
    setAutoDelete(true);
}

void ImageRunnable::run() {
    shared_ptr<ZLImage> image = myNode->image();
    if (!myRequester)
        return;
    QMetaObject::invokeMethod(myRequester.data(), "handleImageResult", Qt::QueuedConnection,
                              Q_ARG(const ZLTreeTitledNode*, myNode), Q_ARG(shared_ptr<ZLImage>, image));
}


ImageProvider::ImageProvider(Mode mode, QObject* parent) : QObject(parent), myMode(mode) {

    qRegisterMetaType<const ZLTreeTitledNode*>();
    qRegisterMetaType<shared_ptr<ZLImage> >();

    myEmptyPixmap = QPixmap(getImageSize());
    myEmptyPixmap.fill(Qt::transparent);

    connect(&myManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRequestFinished(QNetworkReply*)));
}

QPixmap ImageProvider::getImageForNode(const ZLTreeTitledNode *titledNode) {
    if (!myProcessedNodes.contains(titledNode)) {
        QThreadPool::globalInstance()->start(new ImageRunnable(titledNode, this));
        return myEmptyPixmap;
    }
    if (!myCache.contains(generateUrl(titledNode).toString())) {
        return myEmptyPixmap;
    }
    return myCache.value(generateUrl(titledNode).toString());
}

QSize ImageProvider::getImageSize() const {
    return myMode == THUMBNAIL ? MenuItemParameters::getImageSize() : QSize();
}

void ImageProvider::updateCache(QString cacheUrl, QPixmap pixmap) {
    myCache[cacheUrl] = pixmap.isNull() ? myEmptyPixmap : pixmap;
    emit cacheUpdated();
}

QUrl ImageProvider::generateUrl(const ZLTreeTitledNode* node) {
    QString imageUrl = QString::fromStdString(node->imageUrl());
    if (imageUrl.isEmpty()) {
        imageUrl = QString::fromStdString(ZLTreeTitledNode::ZLIMAGE_SCHEME + ZLTreeTitledNode::SCHEME_POSTFIX) +
                   QString::number(reinterpret_cast<qptrdiff>(node), 16);
    }
    return QUrl::fromEncoded(imageUrl.toStdString().c_str());
}

void ImageProvider::handleImageResult(const ZLTreeTitledNode* titledNode, shared_ptr<ZLImage> image) {
    Q_UNUSED(image);
    myProcessedNodes.insert(titledNode);
    QUrl url = generateUrl(titledNode);

    if (myCache.contains(url.toString())) {
        emit cacheUpdated();
        return;
    }

    if (url.scheme() == QLatin1String(ZLTreeTitledNode::ZLIMAGE_SCHEME.c_str())) {
        if (titledNode->image().isNull()) {
            qDebug() << "situation where ZLImage is 0, and link to ZLImage";
            updateCache(url.toString(), QPixmap());
            return;
        }
        updateCache(url.toString(), getZLImage(titledNode->image()));
        return;
    } else if (url.scheme() == QLatin1String(ZLTreeTitledNode::LOCALFILE_SCHEME.c_str())) {
        updateCache(url.toString(), getFSImage(url));
        return;
    } else {
        getNetworkImage(url);
        return;
    }

    return;
}

QPixmap ImageProvider::getZLImage(shared_ptr<ZLImage> image) const {
    return ImageUtils::ZLImageToQPixmap(image, 0, getImageSize());
}

QPixmap ImageProvider::getFSImage(QUrl url) const {
    return ImageUtils::fileUrlToQPixmap(url, 0, getImageSize(), Qt::SmoothTransformation);
}

void ImageProvider::getNetworkImage(QUrl url) const {
    if (!url.isValid()) {
        return;
    }
    QNetworkRequest request(url);
    request.setPriority(myMode == THUMBNAIL ? QNetworkRequest::LowPriority : QNetworkRequest::HighPriority);
    myManager.get(request);
    //TODO maybe use qt standart cache instead of ImageProvider's (for network images)
    //and do not use cache (issue #69) to avoid memory errors
    return;
}

void ImageProvider::onRequestFinished(QNetworkReply* reply) {
    if (myCache.contains(reply->url().toString())) {
        emit cacheUpdated();
        return;
    }

    QPixmap pixmap;
    pixmap.loadFromData(reply->readAll());
    if (!pixmap.isNull()) {
        pixmap = ImageUtils::scaleAndCenterPixmap(pixmap, getImageSize(), true, Qt::SmoothTransformation);
    }
    updateCache(reply->url().toString(), pixmap);
}

