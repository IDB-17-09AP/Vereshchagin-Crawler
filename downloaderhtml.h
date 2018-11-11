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
    Reply getHTML(const QString &url);

    QString contentType() const;
    void setContentType(const QString &value);

signals:
    void errorMessage(const QString &str);

private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QEventLoop loop;
    QString contentType_;
    int count;
};

#endif // DOWNLOADERHTML_H
