#include "crawler.h"

Crawler::Crawler(QWidget *parent)
    : QDialog(parent)
{
    // init
    inputUrl = new QLineEdit("http://stankin.ru");
    inputDepth = new QSpinBox;
    buttonStart = new QPushButton("Start");
    progress = new QProgressBar;
    labelStatus = new QLabel("Ready for work");
    console = new QPlainTextEdit;

    // settings
    inputDepth->setValue(0);
    inputDepth->setRange(0, 100);

    progress->setRange(0, 100);
    progress->setAlignment(Qt::AlignCenter);

    console->setReadOnly(true);
    console->setMinimumSize(350, 200);

    downloader.setContentType("text/html");

    setMinimumSize(350, 300);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    // layout setup
    QHBoxLayout* horLayout = new QHBoxLayout;
    horLayout->addWidget(new QLabel(tr("Link:")));
    horLayout->addWidget(inputUrl);
    horLayout->addWidget(new QLabel(tr("Depth:")));
    horLayout->addWidget(inputDepth);
    horLayout->addWidget(buttonStart);

    QVBoxLayout* verLayout = new QVBoxLayout;
    verLayout->addLayout(horLayout);
    verLayout->addWidget(labelStatus);
    verLayout->addWidget(progress);
    verLayout->addWidget(console);

    setLayout(verLayout);

    // connection
    connect(buttonStart, &QPushButton::clicked, this, &Crawler::start);
    connect(&downloader, &DownloaderHTML::errorMessage, this, &Crawler::writeToConsole);
    connect(&sleepTimer, &QTimer::timeout, &sleepLoop, &QEventLoop::quit);
}

void Crawler::start()
{
    try {
        buttonStart->setEnabled(false);
        inputDepth->setEnabled(false);
        inputUrl->setEnabled(false);
        labelStatus->setText(tr("Please wait..."));
        QFile file(getDomainName()
                   + QDateTime::currentDateTime().toString(" - hh.mm.ss - dd.MM.yyyy")
                   + ".txt");
        if (!file.open(QFile::WriteOnly | QFile::Text))
            throw std::logic_error(file.errorString().toStdString());

        result.setDevice(&file);
        links.insert(inputUrl->text());
        recursionStart(0, downloader.getHTML(inputUrl->text()));
        file.close();

        progress->setValue(100);
        labelStatus->setText(tr("All ready"));

    } catch (std::exception &ex) {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit(ex.what()));
        labelStatus->setText("Error: " + QString::fromLocal8Bit(ex.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", "Unknow error");
        labelStatus->setText(tr("Unknow error"));
    }
    buttonStart->setEnabled(true);
    inputDepth->setEnabled(true);
    inputUrl->setEnabled(true);
}

void Crawler::writeToConsole(const QString &str)
{
    console->appendPlainText(QTime::currentTime().toString("[hh.mm.ss] ") + str);
    console->verticalScrollBar()->setValue(console->verticalScrollBar()->maximum());
}

void Crawler::recursionStart(const int depth, Reply &&reply,
                             const std::shared_ptr<RobotsFile> &file)
{
    if (reply.data.isEmpty())
        return;

    writeToConsole("load link: " + reply.url);

    if (depth < inputDepth->value()) {
        std::shared_ptr<RobotsFile> indexFile;
        if (file != nullptr)
            indexFile = file;
        else {
            QRegExp reg(R"reg(http(s?)://([\w]+\.){1}([\w]+\.?)+)reg");
            reg.indexIn(reply.url);
            indexFile = std::make_shared<RobotsFile>(reply.url.left(reg.matchedLength()));
            writeToConsole("read from: " + indexFile->domain() + "/robots.txt");
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
                progress->setValue(static_cast<int>((first * 100) / reply.data.size()));

            if (!site.isEmpty()) {
                if (indexFile->allow(site) || !indexFile->disallow(site)) {

                    sleepTimer.start((indexFile->delay() + 1) * 1000);      // + 1 т.к. сайт Cтанкина все равно
                    sleepLoop.exec();                                       // банит с задержкой в 2 секунды

                    if (site.startsWith(indexFile->domain()))
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
    if (link.isEmpty())
        return "";     // пустая ссылка

    if (link.contains(QRegExp(".css|.pdf|.png|.docx")))
        return "";     // ссылка на файл

    QString str = link;

    if (str.startsWith("/"))
        str = domain + str;     // ссылка без домена

    if (!str.contains(QRegExp("https?")))
        return "";     // не http(s) ссылка

    str.replace("www.", "");    // убираем www.
    str.replace(QRegularExpression("(?<!:)(\\/\\/)"), "/");

    int index = 0;
    if ((index = str.indexOf(QRegExp(R"(\:\d{2,4})"))) != 0) {
        QString temp = str.left(index);     // ссылка без порта
        if (links.contains(temp))           // уже есть
            return "";
    }

    if (links.contains(str)) {
        if (depth < inputDepth->value())
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

DataOfPage Crawler::stemmingAndIndexing(const QString &text)
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
    QString url = inputUrl->text();
    int first = url.indexOf("//") + 2;
    int second = url.indexOf("/", first);
    if (second == -1)
        return url.mid(first);
    return url.mid(first, second - 1);
}

RobotsFile::RobotsFile(const QString &domain) : delay_(3)
{
    DownloaderHTML loader;
    loader.setContentType("text/plain");
    reg.setPatternSyntax(QRegExp::Wildcard);

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

