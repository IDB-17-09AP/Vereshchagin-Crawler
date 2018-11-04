#ifndef CRAWLER_H
#define CRAWLER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QRegularExpression>
#include <QSet>
#include <QThread>
#include <QTextStream>
#include <QMap>
#include <QDateTime>
#include <memory>

#include "downloaderhtml.h"
#include "stemming.h"

struct DataOfPage
{
    size_t wordNumber = 0;
    QMap<QString, int> wordData;
};

class RobotsFile;

class Crawler : public QObject
{
    Q_OBJECT
public:
    Crawler(QObject *parent = nullptr);
    Crawler(const QString& url, QObject *parent = nullptr, int depth = 2);

    void setMaximumDepth(const int depth);
    void setUrl(const QString &url);
    QString start();

signals:
    void signalProgress(int progress);

private:
    void recursionStart(const int depth, Reply &&reply, const std::shared_ptr<RobotsFile> &file = nullptr);
    QString normalizationLink(const QString &link, const QString &domain, const int depth);
    void parsingText(QString &text);
    DataOfPage stemmingAndIndexing(QString &text);
    QString getDomainName();

    QSet<QString> links;
    QTextStream result;
    DownloaderHTML downloader;
    QString url_;
    int depth_;
};

class RobotsFile
{
public:
    RobotsFile(const QString &domain);

    bool allow(const QString &url);
    bool disallow(const QString &url);
    int delay() const;
    QString domain() const;
    void setdomain(const QString &domain);

private:
    QRegExp reg;
    int delay_;
    QVector<QString> allow_;
    QVector<QString> disallow_;
    QString domain_;
};

#endif // CRAWLER_H
