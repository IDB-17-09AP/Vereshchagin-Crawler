#include "downloaderhtml.h"

DownloaderHTML::DownloaderHTML(QObject *parent)
    : QObject(parent), count_(0)
{
    manager = new QNetworkAccessManager();    
    connect(manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
}

QString DownloaderHTML::getHTML(const QString &url)
{
    count_ = 0;
    QNetworkReply* reply;
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
        QString str(reply->readAll());
        reply->deleteLater();
        return str;
    }
    return "";
}
