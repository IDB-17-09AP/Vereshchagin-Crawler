#include "crawler.h"

Crawler::Crawler(QObject *parent)
    : QObject(parent), delay_(3), depth_(0)
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

        recursionStart(0, url_);
        emit signalProgress(100);
        result.close();
    } catch (...) {
        result.close();
        throw;
    }
}

void Crawler::recursionStart(const int depth, const QString &domen,
                             const std::shared_ptr<RobotsFile> &file)
{
    QString data(downloader.getHTML(domen));

    if (data.size() == 0)
        return;

    if (depth < depth_) {
        std::shared_ptr<RobotsFile> file_;
        if (file != nullptr)
            file_ = file;
        else
            file_ = std::make_shared<RobotsFile>(domen);

        int first = 0, second = 0;
        while (true) {
            first = data.indexOf("href=", second);
            if (first == -1)
                break;

            first += 6;
            second = data.indexOf("\"", first);

            QString str = (normalizationLink(data.mid(first, second - first), domen));

            if (depth == 0)
                emit signalProgress(static_cast<int>((first * 100) / data.size()));

            if (str.size()) {
                if (file_->allow(str) || !file_->disallow(str)) {
                    QThread::sleep(delay_);
                    qDebug() << "Link: " << str;
                    if (str.contains(domen))
                        recursionStart(depth + 1, str, file_);
                    else
                        recursionStart(depth + 1, str);
                }
            }
        }
    }
    parsingText(data);
    stemmingText(data);

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

    str = link;

    if (str[0] == "/")
        str = domen + str;     // ссылка без домена

    if (!str.contains(QRegExp("https?")))
        return "";     // не http(s) ссылка

    if (links.contains(str))
        return "";     // такая ссылка уже была

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

void Crawler::stemmingText(QString &text)
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

        data += Stemming::getStemmedForm(word) + " ";
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


    if (!(removeEndingWord(rv, PERFECTIVE_1, true) || removeEndingWord(rv, PERFECTIVE_2)))
        removeEndingWord(rv, REFLEXIVE);
        if (!(removeEndingWord(rv, ADJECTIVE) || removeEndingWord(rv, ADJECTIVAL_1, true)
              || removeEndingWord(rv, ADJECTIVAL_2)))
            if (!(removeEndingWord(rv, VERB_1, true) || removeEndingWord(rv, VERB_2)))
                removeEndingWord(rv, NOUN);


    if (rv.indexOf("и", rv.size() - 1) != -1)
        rv = rv.remove(rv.size() - 1, 1);


    {
        QRegExp reg(VOWER + "\\w");
        int index = reg.indexIn(rv);
        QString r2 = rv.mid(index + 2);

        if (removeEndingWord(r2, DERIVATIONAL))
            removeEndingWord(rv, DERIVATIONAL);
    }

    if (rv.indexOf("нн", rv.size() - 2) != -1)
        rv = rv.remove(rv.size() - 2, 2);

    removeEndingWord(rv, SUPERLATIVE);

    if (rv.indexOf("нн", rv.size() - 2) != -1)
        rv = rv.remove(rv.size() - 2, 2);

    if (rv.indexOf("ь", rv.size() - 1) != -1)
        rv = rv.remove(rv.size() - 1, 1);


    return  std::move(pre + rv);
}


bool Stemming::removeEndingWord(QString &word, const QVector<QString> category, bool inGroup)
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
// const QVector<QString> Stemming::NO_WORD = {""};
const QString Stemming::VOWER = "[аеиоуыэюя]";
const QVector<QString> Stemming::PERFECTIVE_1 = {"в", "вши", "вшись"};
const QVector<QString> Stemming::PERFECTIVE_2 = {"ив", "ивши", "ившись", "ыв", "ывши", "ывшись"};
const QVector<QString> Stemming::ADJECTIVE = {"ее", "ие", "ые", "ое", "ими", "ыми", "ей", "ий",
                                 "ый", "ой", "ем", "им", "ым", "ом", "его", "ого",
                                 "ему", "ому", "их", "ых", "ую", "юю", "ая", "яя", "ою", "ею"};
const QVector<QString> Stemming::PARTICIPLE_1 = {"ем", "нн", "вш", "ющ", "щ"};
const QVector<QString> Stemming::PARTICIPLE_2 = {"ивш", "ывш", "ующ"};
const QVector<QString> Stemming::ADJECTIVAL_1 = {"емее", "ннее", "вшее", "ющее", "щее",
                                              "емие", "нние", "вшие", "ющие", "щие",
                                              "емые", "нные", "вшые", "ющые", "щые",
                                              "емое", "нное", "вшое", "ющое", "щое",
                                              "емими", "нними", "вшими", "ющими", "щими",
                                              "емыми", "нными", "вшыми", "ющыми", "щыми",
                                              "емей", "нней", "вшей", "ющей", "щей",
                                              "емий", "нний", "вший", "ющий", "щий",
                                              "емый", "нный", "вшый", "ющый", "щый",
                                              "емой", "нной", "вшой", "ющой", "щой",
                                              "емем", "ннем", "вшем", "ющем", "щем",
                                              "емим", "нним", "вшим", "ющим", "щим",
                                              "емым", "нным", "вшым", "ющым", "щым",
                                              "емом", "нном", "вшом", "ющом", "щом",
                                              "емего", "ннего", "вшего", "ющего", "щего",
                                              "емого", "нного", "вшого", "ющого", "щого",
                                              "емему", "ннему", "вшему", "ющему", "щему",
                                              "емому", "нному", "вшому", "ющому", "щому",
                                              "емих", "нних", "вших", "ющих", "щих",
                                              "емых", "нных", "вшых", "ющых", "щых",
                                              "емую", "нную", "вшую", "ющую", "щую",
                                              "емюю", "ннюю", "вшюю", "ющюю", "щюю",
                                              "емая", "нная", "вшая", "ющая", "щая",
                                              "емяя", "нняя", "вшяя", "ющяя", "щяя",
                                              "емою", "нною", "вшою", "ющою", "щою",
                                              "емею", "ннею", "вшею", "ющею", "щею"};
const QVector<QString> Stemming::ADJECTIVAL_2 = {"ившее", "ывшее", "ующее",
                                              "ившие", "ывшие", "ующие",
                                              "ившые", "ывшые", "ующые",
                                              "ившое", "ывшое", "ующое",
                                              "ившими", "ывшими", "ующими",
                                              "ившыми", "ывшыми", "ующыми",
                                              "ившей", "ывшей", "ующей",
                                              "ивший", "ывший", "ующий",
                                              "ившый", "ывшый", "ующый",
                                              "ившой", "ывшой", "ующой",
                                              "ившем", "ывшем", "ующем",
                                              "ившим", "ывшим", "ующим",
                                              "ившым", "ывшым", "ующым",
                                              "ившом", "ывшом", "ующом",
                                              "ившего", "ывшего", "ующего",
                                              "ившого", "ывшого", "ующого",
                                              "ившему", "ывшему", "ующему",
                                              "ившому", "ывшому", "ующому",
                                              "ивших", "ывших", "ующих",
                                              "ившых", "ывшых", "ующых",
                                              "ившую", "ывшую", "ующую",
                                              "ившюю", "ывшюю", "ующюю",
                                              "ившая", "ывшая", "ующая",
                                              "ившяя", "ывшяя", "ующяя",
                                              "ившою", "ывшою", "ующою",
                                              "ившею", "ывшею", "ующею"};
const QVector<QString> Stemming::REFLEXIVE = {"ся", "сь"};
const QVector<QString> Stemming::VERB_1 = {"ла", "на", "ете", "йте", "ли", "й", "л", "ем", "н", "ло", "но",
                                        "ет", "ют", "ны", "ть", "ешь", "нно"};
const QVector<QString> Stemming::VERB_2 = {"ила", "ыла", "ена", "ейте", "уйте", "ите", "или", "ыли", "ей", "уй", "ил",
                                        "ыл", "им", "ым", "ен", "ило", "ыло", "ено", "ят",
                                        "ует", "уют", "ит", "ыт", "ены", "ить", "ыть",
                                        "ишь", "ую", "ю"};
const QVector<QString> Stemming::NOUN = {"а", "ев", "ов", "ие", "ье", "е", "иями", "ями", "ами", "еи", "ии", "и", "ией", "ей",
                             "ой", "ий", "й", "иям", "ям", "ием", "ем", "ам", "ом", "о", "у", "ах",
                             "иях", "ях", "ы", "ь", "ию", "ью", "ю", "ия", "ья", "я"};
const QVector<QString> Stemming::SUPERLATIVE = {"ейш", "ейше"};
const QVector<QString> Stemming::DERIVATIONAL = {"ост", "ость"};

RobotsFile::RobotsFile(const QString &domen) : delay_(3)
{
    DownloaderHTML loader;
    reg.setPatternSyntax(QRegExp::Wildcard);
    QString data = loader.getHTML(domen + "/robots.txt");
    QString str = "";

    bool find = false;

    QTextStream input(&data);

    while (!input.atEnd()) {
        input >> str;

        if (find && str == "User-Agent:")
            break;

        if (str == "User-Agent:") {
            input >> str;
            if (str == "*")
                find = true;
        }

        if (find) {
            if (str == "Allow:") {
                input >> str;
                allow_.push_back(str);
            } else if (str == "Disallow:") {
                input >> str;
                disallow_.push_back(str);
            } else if (str == "Crawl-delay:")
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
        if (index != -1) {
            qDebug() << "Wrong:" << url;
            return true;
        }
    }
    return false;
}

int RobotsFile::delay()
{
    return delay_;
}
