#include "crawler.h"

Crawler::Crawler(QObject *parent)
    : QObject(parent), depth_(0)
{
}

Crawler::Crawler(const QString &url, QObject *parent, int depth)
    : QObject(parent), url_(url), depth_(depth)
{
}

void Crawler::setMaximumDepth(const int depth)
{
    depth_ = depth;
}

void Crawler::setUrl(const QString &url)
{
    url_ = url;
}

QString Crawler::start()
{
    try {
        QFile file(getDomainName() + QDateTime::currentDateTime().toString(" - hh.mm.ss - dd.MM.yyyy") + ".txt");
        if (!file.open(QFile::WriteOnly | QFile::Text))
            throw std::logic_error(file.errorString().toStdString());

        result.setDevice(&file);
        recursionStart(0, downloader.getHTML(url_));
        emit signalProgress(100);
        file.close();
        return QFileInfo(file).absoluteDir().absolutePath();
    } catch (...) {
        throw;
    }
}

void Crawler::recursionStart(const int depth, Reply &&reply,
                             const std::shared_ptr<RobotsFile> &file)
{
    if (reply.url.isEmpty())
        return;

    if (depth < depth_) {
        std::shared_ptr<RobotsFile> indexFile;
        if (file != nullptr)
            indexFile = file;
        else {
            QRegExp reg(R"reg(http(s?)://([\w]+\.){1}([\w]+\.?)+)reg");
            reg.indexIn(reply.url);
            indexFile = std::make_shared<RobotsFile>(reply.url.left(reg.matchedLength()));
        }

        int first = 0, second = 0;
        while (true) {
            first = reply.data.indexOf("href=", second);
            if (first == -1)
                break;

            first += 6;
            second = reply.data.indexOf("\"", first);

            QString site = normalizationLink(reply.data.mid(first, second - first), indexFile->domain(), depth);

            if (depth == 0)
                emit signalProgress(static_cast<int>((first * 100) / reply.data.size()));

            if (!site.isEmpty()) {
                if (indexFile->allow(site) || !indexFile->disallow(site)) {
                    QThread::sleep(static_cast<ulong>(indexFile->delay() + 1)); // + 1 т.к. сайт Cтанкина все равно
                    qDebug() << "Link: " << site;                               // банит с задержкой в 2 секунды

                    if (site.contains(indexFile->domain()))
                        recursionStart(depth + 1, downloader.getHTML(site), indexFile);
                    else
                        recursionStart(depth + 1, downloader.getHTML(site));
                }
            }
        }
    }
    parsingText(reply.data);
    auto resultData = stemmingAndIndexing(reply.data);

    // write
    result << reply.url << " " << resultData.wordNumber << " ";

    for (auto it = resultData.wordData.cbegin(); it != resultData.wordData.cend(); ++it)
        result << it.key() << " " << it.value() << " ";

    result << "<endLine>\n";
}

QString Crawler::normalizationLink(const QString &link, const QString& domain, const int depth)
{
    QString str("");

    if (link.isEmpty())
        return str;     // пустая ссылка

    if (link.contains(QRegExp(".css|.pdf|.png|.docx")))
        return str;     // ссылка на файл

    str = link;

    if (str[0] == "/")
        str = domain + str;     // ссылка без домена

    if (!str.contains(QRegExp("https?")))
        return "";     // не http(s) ссылка

    str.replace("www.", "");    // убираем www.
    str.replace(QRegularExpression("(?<!:)(\\/\\/)"), "/");

    int index;
    if ((index = str.indexOf(QRegExp("(?:\\:\\d{2,4})")) + 1) != 0) {
        QString temp = str.left(index);     // ссылка без порта
        if (links.contains(temp))           // уже есть
            return "";
    }

    if (links.contains(str)) {
        if (depth < depth_)
            return str;
        else
            return "";     // такая ссылка уже была
    }

    links.insert(str);
    return str;
}

void Crawler::parsingText(QString &text)
{
    QRegExp reg("(<scrip(.*)<\\/script>)|(<style(.*)<\\/style>)"
                "|(<!--(.*)-->)|(<pre(.*)<\\/pre>)");
    reg.setMinimal(true);

    text.replace(reg, " "); // вырезаем: скрипты, стили,
                            // комментарии и pre теги
    reg.setPattern("(<(.*)>)");
    text.replace(reg, " "); // вырезаем все остальные теги

    reg.setPattern("&\\w{0,7};");
    text.replace(reg, " "); // вырезаем спец. символы
}

DataOfPage Crawler::stemmingAndIndexing(QString &text)
{
    int index = 0;
    QRegExp reg("([А-Яа-я])+");

    QString word;
    DataOfPage data;

    while (true) {
        index = reg.indexIn(text, index);
        if (index == -1)
            break;
        word = text.mid(index, reg.matchedLength()).toLower();
        index += reg.matchedLength();

        // Cтемминг
        // https://ru.wikipedia.org/wiki/Стеммер_Портера

        word = Stemming::getStemmedForm(word);
        if (!word.isEmpty()) {
            ++data.wordData[word];
            ++data.wordNumber;
        }
    }
    return data;
}

QString Crawler::getDomainName()
{
    int first = url_.indexOf("//") + 2;
    int second = url_.indexOf("/", first);
    if (second == -1)
        return url_.mid(first);
    return url_.mid(first, second - 1);
}

RobotsFile::RobotsFile(const QString &domain) : delay_(3)
{
    DownloaderHTML loader;
    reg.setPatternSyntax(QRegExp::Wildcard);
    qDebug() << "Read from: " << domain + "/robots.txt";
    domain_ = domain;
    QString data = loader.getHTML(domain + "/robots.txt").data;
    QString str = "";

    bool find = false;

    QTextStream input(&data);

    while (!input.atEnd()) {
        input >> str;
        str = str.toLower();

        if (find && str == "user-agent:")
            break;

        if (str == "user-agent:") {
            input >> str;
            if (str == "*")
                find = true;
        }

        if (find) {
            if (str == "allow:") {
                input >> str;
                allow_.push_back(str);
            } else if (str == "disallow:") {
                input >> str;
                disallow_.push_back(str);
            } else if (str == "crawl-delay:")
                input >> delay_;
        }
    }
}

bool RobotsFile::allow(const QString &url)
{
    int index = 0;
    for (const auto& var : allow_) {
        reg.setPattern(var);
        index = reg.indexIn(url);
        if (index != -1)
            return true;
    }
    return false;
}

bool RobotsFile::disallow(const QString &url)
{
    int index = 0;
    for (const auto& var : disallow_) {
        reg.setPattern(var);
        index = reg.indexIn(url);
        if (index != -1)
            return true;
    }
    return false;
}

int RobotsFile::delay() const
{
    return delay_;
}

QString RobotsFile::domain() const
{
    return domain_;
}

void RobotsFile::setdomain(const QString &domain)
{
    domain_ = domain;
}
