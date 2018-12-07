#include "downloaderhtml.h"

DownloaderHTML::DownloaderHTML(QObject *parent)
    : QObject(parent), count(0)
{
    // init
    manager = new QNetworkAccessManager();

    // settings
    request.setAttribute(QNetworkRequest::Attribute::FollowRedirectsAttribute, true);
    request.setMaximumRedirectsAllowed(5);

    // connection
    connect(manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
}
Reply DownloaderHTML::getHTML(const QString &url)
{
    request.setUrl(QUrl(url));

    auto networkReplyHead = manager->head(request);
    loop.exec();

//    qDebug() << networkReplyHead->header(QNetworkRequest::ContentTypeHeader).toString();

    if (networkReplyHead->header(QNetworkRequest::ContentTypeHeader).toString().startsWith(contentType_) ||
            networkReplyHead->header(QNetworkRequest::ContentTypeHeader).toString().isEmpty()) {
        QNetworkReply *networkReply;
        request.setUrl(networkReplyHead->url());
        count = 0;
        do {
            networkReply = manager->get(request);
            loop.exec();
            if (networkReply->error() == QNetworkReply::TimeoutError)
                ++count;
            else
                break;
        } while (count < 3);

        if (networkReply->error() == QNetworkReply::NetworkSessionFailedError) {
            throw std::runtime_error(networkReply->errorString().toStdString());
        } else if (networkReply->error() == QNetworkReply::NoError) {
            QByteArray type = networkReply->rawHeader("Content-Type");
            int index = type.indexOf("charset=");
            QTextCodec *codec = QTextCodec::codecForName(index >= 0 ? type.mid(index + 8) : "UTF-8");
            Reply reply = {networkReply->url().toString(), codec->toUnicode(networkReply->readAll())};
            networkReply->deleteLater();
            return reply;
        }
        emit errorMessage("error: " + networkReply->errorString());
    }
    return {};
}

QString DownloaderHTML::contentType() const
{
    return contentType_;
}

void DownloaderHTML::setContentType(const QString &value)
{
    contentType_ = value;
}
