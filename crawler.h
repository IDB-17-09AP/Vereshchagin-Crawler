#ifndef CRAWLER_H
#define CRAWLER_H

#include <QObject>
#include <QFile>
#include <QRegExp>
#include <QSet>
#include <QThread>

#include "downloaderhtml.h"

class Stemming;

class Crawler : public QObject
{
    Q_OBJECT
public:
    Crawler(QObject *parent = nullptr);
    Crawler(const QString& url, QObject *parent = nullptr, int depth = 2, int delay = 2);

    void setDelay(const int delay);
    void setMaximumDepth(const int depth);
    void setUrl(const QString &url);
    void start();

signals:
    void signalProgress(int progress);

private:
    void recursionStart(const int depth, const QString &domen, Stemming &stem);
    QString normalizationLink(const QString &link, const QString &domen);
    void parsingText(QString &text);
    void stemmingText(QString &text, Stemming &stem);

    QSet<QString> links;
    QFile result;
    DownloaderHTML downloader;
    QString url_;
    int delay_;
    int depth_;
};

class Stemming
{
public:
    Stemming() = default;
    QString getStemmedForm(QString &word);

private:
    bool removeEndingWord(QString &word, QVector<QString> category, bool inGroup = false);
    QString toR2(QString &str);

    const QString VOWER = "[аеиоуыэюя]";
    const QVector<QString> PERFECTIVE_1 = {"в", "вши", "вшись"};
    const QVector<QString> PERFECTIVE_2 = {"ив", "ивши", "ившись", "ыв", "ывши", "ывшись"};
    const QVector<QString> ADJECTIVE = {"ее", "ие", "ые", "ое", "ими", "ыми", "ей", "ий",
                                     "ый", "ой", "ем", "им", "ым", "ом", "его", "ого",
                                     "ему", "ому", "их", "ых", "ую", "юю", "ая", "яя", "ою", "ею"};
    const QVector<QString> PARTICIPLE_1 = {"ем", "нн", "вш", "ющ", "щ"};
    const QVector<QString> PARTICIPLE_2 = {"ивш", "ывш", "ующ"};
    const QVector<QString> ADJECTIVAL_1 = {"емее", "ннее", "вшее", "ющее", "щее",
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
    const QVector<QString> ADJECTIVAL_2 = {"ившее", "ывшее", "ующее",
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
    const QVector<QString> REFLEXIVE = {"ся", "сь"};
    const QVector<QString> VERB_1 = {"ла", "на", "ете", "йте", "ли", "й", "л", "ем", "н", "ло", "но",
                                            "ет", "ют", "ны", "ть", "ешь", "нно"};
    const QVector<QString> VERB_2 = {"ила", "ыла", "ена", "ейте", "уйте", "ите", "или", "ыли", "ей", "уй", "ил",
                                            "ыл", "им", "ым", "ен", "ило", "ыло", "ено", "ят",
                                            "ует", "уют", "ит", "ыт", "ены", "ить", "ыть",
                                            "ишь", "ую", "ю"};
    const QVector<QString> NOUN = {"а", "ев", "ов", "ие", "ье", "е", "иями", "ями", "ами", "еи", "ии", "и", "ией", "ей",
                                 "ой", "ий", "й", "иям", "ям", "ием", "ем", "ам", "ом", "о", "у", "ах",
                                 "иях", "ях", "ы", "ь", "ию", "ью", "ю", "ия", "ья", "я"};
    const QVector<QString> SUPERLATIVE = {"ейш", "ейше"};
    const QVector<QString> DERIVATIONAL = {"ост", "ость"};
};

#endif // CRAWLER_H
