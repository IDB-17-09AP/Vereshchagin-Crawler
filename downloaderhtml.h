#ifndef DOWNLOADERHTML_H
#define DOWNLOADERHTML_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QObject>
#include <QEventLoop>
#include <QTextCodec>

struct Reply
{
    QString url;
    QString data;
};

class DownloaderHTML : public QObject
{
    Q_OBJECT
public:
    explicit DownloaderHTML(QObject *parent = nullptr);
    Reply getHTML(const QString &url, int redirection = 3);

private:
    QNetworkAccessManager *manager;
    QEventLoop loop;
    int count_;
};

#endif // DOWNLOADERHTML_H
