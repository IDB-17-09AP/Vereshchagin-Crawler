#ifndef STEMMING_H
#define STEMMING_H

#include <QtWidgets>

class Stemming
{
public:
    static QString getStemmedForm(const QString &word);

private:
    Stemming();
    Stemming(const Stemming &other) = delete;
    Stemming& operator =(const Stemming &other) = delete;

    static bool removeEndingWord(QString &word, const QVector<QString> category, bool inGroup = false);

    static const QSet<QString> STOP_WORD;
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


class StemmingEng
{
public:
    static QString getStemmedForm(QString &word);

private:
    StemmingEng();
    StemmingEng(const StemmingEng &other) = delete;
    StemmingEng& operator =(const StemmingEng &other) = delete;

    static bool special(QString &word);
    static void changeY(QString &word);
    static bool isVowel(QChar ch);
    static bool isVowelY(QChar ch);
    static int firstNonVowelAfterVowel(QString &word, int start);
    static int getStartR1(QString &word);
    static int getStartR2(QString &word, int startR1);

    static bool replaceIfExists(QString &word, QString suffix, QString replacement, int start);
    static bool containsVowel(QString &word, int start, int end);
    static bool endsInDouble(const QString &word);
    static bool isShort(const QString &word);
    static bool isValidLIEnding(QChar ch);

    static void step0(QString &word);
    static bool step1A(QString &word);
    static void step1B(QString &word, int startR1);
    static void step1C(QString &word);
    static void step2(QString &word, int startR1);
    static void step3(QString &word, int startR1, int startR2);
    static void step4(QString &word, int startR2);
    static void step5(QString &word, int startR1, int startR2);
};

#endif // STEMMING_H
