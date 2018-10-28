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
    QMap<QString, int> stemmingAndIndexing(QString &text);
    QString getDomainName();

    QSet<QString> links;
    QTextStream result;
    DownloaderHTML downloader;
    QString url_;
    int depth_;
};


class Stemming
{
public:
    static QString getStemmedForm(QString &word);

private:
    Stemming() {}
    Stemming(const Stemming &other) = delete;
    Stemming& operator =(const Stemming &other) = delete;

    static bool removeEndingWord(QString &word, const QVector<QString> category, bool inGroup = false);

//    static const QVector<QString> NO_WORD;

    static const QString VOWER;
    static const QVector<QString> PERFECTIVE_1;
    static const QVector<QString> PERFECTIVE_2;
    static const QVector<QString> ADJECTIVE;
    static const QVector<QString> PARTICIPLE_1;
    static const QVector<QString> PARTICIPLE_2;
    static const QVector<QString> ADJECTIVAL_1;
    static const QVector<QString> ADJECTIVAL_2;
    static const QVector<QString> REFLEXIVE;
    static const QVector<QString> VERB_1;
    static const QVector<QString> VERB_2;
    static const QVector<QString> NOUN;
    static const QVector<QString> SUPERLATIVE;
    static const QVector<QString> DERIVATIONAL;
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
