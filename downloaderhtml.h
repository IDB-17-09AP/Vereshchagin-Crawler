#ifndef DOWNLOADERHTML_H
#define DOWNLOADERHTML_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QObject>
#include <QEventLoop>

class DownloaderHTML : public QObject
{
    Q_OBJECT
public:
    explicit DownloaderHTML(QObject *parent = nullptr);
    QString getHTML(const QString &url);

private:
    QNetworkAccessManager *manager;
    QEventLoop loop;
    int count_;
};

#endif // DOWNLOADERHTML_H
