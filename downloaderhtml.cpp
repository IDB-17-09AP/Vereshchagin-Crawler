#include "downloaderhtml.h"

DownloaderHTML::DownloaderHTML(QObject *parent)
    : QObject(parent), count_(0)
{
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
}
Reply DownloaderHTML::getHTML(const QString &url, int redirection)
{
    count_ = 0;
    QNetworkReply *networkReply;
    do {
        networkReply = manager->get(QNetworkRequest(QUrl(url)));
        loop.exec();
        if (networkReply->error() == QNetworkReply::TimeoutError)
            ++count_;
        else
            break;
    } while (count_ < 3);

    if (networkReply->error() == QNetworkReply::NetworkSessionFailedError) {
        throw std::runtime_error(networkReply->errorString().toStdString());
    } else if (networkReply->error() == QNetworkReply::NoError){
        QString location = networkReply->rawHeader("Location");
        if (location.isEmpty()) {
            QByteArray type = networkReply->rawHeader("Content-Type");
            int index = type.indexOf("charset=");
            QTextCodec *codec = QTextCodec::codecForName(index >= 0 ? type.mid(index + 8) : "UTF-8");
            Reply reply = {url, codec->toUnicode(networkReply->readAll())};
            networkReply->deleteLater();
            return reply;
        }
        if (redirection > 0)
            return getHTML(location, --redirection);
    }
    qDebug() << "Error getting HTML:\t" << networkReply->errorString();
    return {};
}
