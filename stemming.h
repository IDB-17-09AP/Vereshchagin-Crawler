#ifndef STEMMING_H
#define STEMMING_H

#include <QString>
#include <QVector>
#include <QSet>
#include <QRegExp>

class Stemming
{
public:
    static QString getStemmedForm(const QString &word);

private:
    Stemming() {}
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

#endif // STEMMING_H
