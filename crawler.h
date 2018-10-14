#ifndef CRAWLER_H
#define CRAWLER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QSet>
#include <QThread>
#include <QTextStream>
#include <QMap>
#include <memory>

#include "downloaderhtml.h"

class RobotsFile;

class Crawler : public QObject
{
    Q_OBJECT
public:
    Crawler(QObject *parent = nullptr);
    Crawler(const QString& url, QObject *parent = nullptr, int depth = 2, int delay = 2);

    void setDelay(const int delay);
    void setMaximumDepth(const int depth);
    void setUrl(const QString &url);
    QString start();

signals:
    void signalProgress(int progress);

private:
    void recursionStart(const int depth, const QString &domen, const std::shared_ptr<RobotsFile> &file = nullptr);
    QString normalizationLink(const QString &link, const QString &domen);
    void parsingText(QString &text);
    QMap<QString, int> stemmingText(QString &text);

    QSet<QString> links;
    QTextStream result;
    DownloaderHTML downloader;
    QString url_;
    int delay_;
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
    RobotsFile(const QString &domen);

    bool allow(const QString &url);
    bool disallow(const QString &url);
    int delay();
private:
    QRegExp reg;
    int delay_;
    QVector<QString> allow_;
    QVector<QString> disallow_;
};

#endif // CRAWLER_H
