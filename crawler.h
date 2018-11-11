#ifndef CRAWLER_H
#define CRAWLER_H

#include <QtWidgets>

#include "downloaderhtml.h"
#include "stemming.h"

struct DataOfPage;
class RobotsFile;

class Crawler : public QDialog
{
    Q_OBJECT
public:
    Crawler(QWidget *parent = nullptr);

public slots:
    void start();
    void writeToConsole(const QString &str);

private:
    void recursionStart(const int depth, Reply &&reply, const std::shared_ptr<RobotsFile> &file = nullptr);
    QString normalizationLink(const QString &link, const QString &domain, const int depth);
    void parsingText(QString &text);
    DataOfPage stemmingAndIndexing(const QString &text);
    QString getDomainName();

    QLineEdit *inputUrl;
    QLabel *labelStatus;
    QPushButton *buttonStart;
    QSpinBox *inputDepth;
    QProgressBar* progress;
    QPlainTextEdit *console;

    QSet<QString> links;
    QTextStream result;
    DownloaderHTML downloader;
    QTimer sleepTimer;
    QEventLoop sleepLoop;
};

struct DataOfPage
{
    size_t wordNumber = 0;
    QMap<QString, int> wordData;
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
