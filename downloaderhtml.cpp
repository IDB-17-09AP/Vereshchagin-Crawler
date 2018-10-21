#include "downloaderhtml.h"

DownloaderHTML::DownloaderHTML(QObject *parent)
    : QObject(parent), count_(0)
{
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
}

QString DownloaderHTML::getHTML(const QString &url, bool redirection)
{
    count_ = 0;
    QNetworkReply *reply;
    do {
        reply = manager->get(QNetworkRequest(QUrl(url)));
        loop.exec();
        if (reply->error() == QNetworkReply::TimeoutError)
            ++count_;
        else
            break;
    } while (count_ < 3);

    if (reply->error() == QNetworkReply::NetworkSessionFailedError) {
        throw std::runtime_error(reply->errorString().toStdString());
    } else if (reply->error() == QNetworkReply::NoError){
        QString location = reply->rawHeader("Location");
        if (location.isEmpty()) {
            QByteArray type = reply->rawHeader("Content-Type");
            int index = type.indexOf("charset=");
            QTextCodec *codec = QTextCodec::codecForName(index >= 0 ? type.mid(index + 8) : "UTF-8");
            QString str = codec->toUnicode(reply->readAll());
            reply->deleteLater();
            return str;
        }
        if (!redirection)
            return getHTML(location, true);
    }
    qDebug() << "Error getting HTML:\t" << reply->errorString();
    return "";
}
