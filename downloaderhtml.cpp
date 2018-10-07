#include "downloaderhtml.h"

DownloaderHTML::DownloaderHTML(QObject *parent)
    : QObject(parent)
{
    manager = new QNetworkAccessManager();    
}

QString DownloaderHTML::getHTML(const QString &url)
{
    auto reply = manager->get(QNetworkRequest(QUrl(url)));
    QEventLoop loop;
    connect(manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if(reply->error() != QNetworkReply::NoError){
        throw std::runtime_error(reply->errorString().toStdString());
    }
    QString str(reply->readAll());
    reply->deleteLater();
    return str;
}
