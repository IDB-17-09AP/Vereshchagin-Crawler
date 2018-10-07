#include "crawler.h"

Crawler::Crawler(QObject *parent)
    : QObject(parent), delay_(2), depth_(0)
{
}

Crawler::Crawler(const QString &url, QObject *parent, int depth, int delay)
    : QObject(parent), url_(url), delay_(depth), depth_(delay)
{
}

void Crawler::setDelay(const int delay)
{
    delay_ = delay;
}

void Crawler::setMaximumDepth(const int depth)
{
    depth_ = depth;
}

void Crawler::setUrl(const QString &url)
{
    url_ = url;
}

void Crawler::start()
{
    try {
        result.setFileName("result.txt");
        if (!result.open(QFile::WriteOnly | QFile::Text | QFile::Append | QFile::Truncate))
            throw std::logic_error(result.errorString().toStdString());

        Stemming stem;
        recursionStart(0, url_, stem);
    } catch (...) {
        throw;
    }

    result.close();
}

void Crawler::recursionStart(const int depth, const QString &domen, Stemming &stem)
{
    QString data(downloader.getHTML(domen));

    if (depth < depth_) {
        int first = 0, second = 0;
        while (true) {
            first = data.indexOf("href=", second);
            if (first == -1)
                break;

            first += 6;
            second = data.indexOf("\"", first);

            QString str = (normalizationLink(data.mid(first, second - first), domen));

            if (domen == 0)
                emit signalProgress(static_cast<int>((data.size() * 100) / first));

            if (str.size()) {
                QThread::sleep(delay_);
                recursionStart(depth + 1, str, stem);
            }
        }
    }
    parsingText(data);
    stemmingText(data, stem);

    result.write(data.toUtf8());
    result.write("<end>");
    result.write(domen.toUtf8());
    result.write("\n");
}

QString Crawler::normalizationLink(const QString &link, const QString& domen)
{
    QString str("");

    if (link.size() == 0)
        return str;     // пустая ссылка

    if (link.contains(QRegExp(".css|.pdf|.png|.docx")))
        return str;     // ссылка на файл

    if (link[0] == "/")
        str = domen + link;     // ссылка без домена

    if (!link.contains(QRegExp("(http[s])")))
        return str;     // не http(s) ссылка

    if (links.contains(str))
        return str;     // такая ссылка уже была

    links.insert(str);
    return str;
}

void Crawler::parsingText(QString &text)
{
    QRegExp reg("(<scrip(.*)<\\/script>)|(<style(.*)<\\/style>)"
                "|(<!--(.*)-->)|(<pre(.*)<\\/pre>)");
    reg.setMinimal(true);

    text.replace(reg, "");  // вырезаем: скрипты, стили,
                            // комментарии и pre теги
    reg.setPattern("(<(.*)>)");
    text.replace(reg, "");  // вырезаем все остальные теги

    reg.setPattern("&\\w{0,7};");
    text.replace(reg, "");  // вырезаем спец. символы
}

void Crawler::stemmingText(QString &text, Stemming &stem)
{
    QString data, word;

    int index = 0;
    QRegExp reg("([А-Яа-я])+");

    while (true) {
        index = reg.indexIn(text, index);
        if (index == -1)
            break;
        word = text.mid(index, reg.matchedLength()).toLower();
        index += reg.matchedLength();

        // Cтемминг
        // https://ru.wikipedia.org/wiki/Стеммер_Портера

        data += stem.getStemmedForm(word) + " ";

//        qDebug() << word;
    }
    text = std::move(data);
}

QString Stemming::getStemmedForm(QString &word)
{
    QRegExp reg(VOWER);

    int index = reg.indexIn(word) + 1;
    if (index == 0)
        return QString("");

    QString rv = word.mid(index);
    QString pre = word.left(index);

//    qDebug() << "rv: " << rv;
//    qDebug() << "pre: " << pre;

    if (!(removeEndingWord(rv, PERFECTIVE_1, true) || removeEndingWord(rv, PERFECTIVE_2))) {
        removeEndingWord(rv, REFLEXIVE);
        if (!(removeEndingWord(rv, ADJECTIVE) || removeEndingWord(rv, ADJECTIVAL_1, true) || removeEndingWord(rv, ADJECTIVAL_2))) {
//            qDebug() << "VERB";
            if (!(removeEndingWord(rv, VERB_1, true) || removeEndingWord(rv, VERB_2))) {
                removeEndingWord(rv, NOUN);
//                qDebug() << "NOUN";
            }
        }
    }
//    qDebug() << "rv #2: " << rv;

    if (rv.indexOf("и", rv.size() - 1) != -1)
        rv = rv.remove(rv.size() - 1, 1);

    removeEndingWord(rv, DERIVATIONAL);

    if (rv.indexOf("нн", rv.size() - 2) != -1)
        rv = rv.remove(rv.size() - 2, 2);

    removeEndingWord(rv, SUPERLATIVE);

    if (rv.indexOf("нн", rv.size() - 2) != -1)
        rv = rv.remove(rv.size() - 2, 2);

    if (rv.indexOf("ь", rv.size() - 1) != -1)
        rv = rv.remove(rv.size() - 1, 1);

//    qDebug() << "rv #3: " << rv;

    return  std::move(pre + rv);
}

bool Stemming::removeEndingWord(QString &word, QVector<QString> category, bool inGroup)
{
    int maxLenght = 0;
    for (const auto& var : category) {
        if (word.size() - var.size() - 1 >= 0)
            if ((word.indexOf(var, word.size() - var.size()) != -1)
                    && (maxLenght < var.size())
                    && (!inGroup || word[word.size() - var.size() - 1] == "а"
                                 || word[word.size() - var.size() - 1] == "я"))
                maxLenght = var.size();
    }
    if (maxLenght) {
        word.remove(word.size() - maxLenght, maxLenght);
        return true;
    }
    return false;
}

QString Stemming::toR2(QString &str)
{
    QRegExp reg(VOWER + "\\w");
    int index = reg.indexIn(str);
    QString r1 = str.mid(index + 2);

    index = reg.indexIn(r1);
    QString r2 = r1.mid(index + 2);

    return  r2;
}
