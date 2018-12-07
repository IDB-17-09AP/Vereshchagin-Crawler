#include "stemming.h"

QString Stemming::getStemmedForm(const QString &word)
{
    if (STOP_WORD.contains(word))
        return "";

    QRegExp reg(VOWER);

    int index = reg.indexIn(word) + 1;
    if (index == 0)
        return "";

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

    reg.setPattern(VOWER + "\\w");
    index = reg.indexIn(rv);
    QString r2 = rv.mid(index + 2);

    if (removeEndingWord(r2, DERIVATIONAL))
        removeEndingWord(rv, DERIVATIONAL);

    if (rv.indexOf("нн", rv.size() - 2) != -1)
        rv = rv.remove(rv.size() - 2, 2);

    removeEndingWord(rv, SUPERLATIVE);

    if (rv.indexOf("нн", rv.size() - 2) != -1)
        rv = rv.remove(rv.size() - 2, 2);

    if (rv.indexOf("ь", rv.size() - 1) != -1)
        rv = rv.remove(rv.size() - 1, 1);


    return pre + rv;
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
const QSet<QString> Stemming::STOP_WORD = {"а", "будем", "будет", "будете", "будешь", "буду", "будут", "будучи",
                                              "будь", "будьте", "бы", "был", "была", "были", "было", "быть",
                                              "в", "вам", "вами", "вас", "весь", "во", "вот", "все",
                                              "всё", "всего", "всей", "всем", "всём", "всеми", "всему", "всех",
                                              "всею", "всея", "всю", "вся", "вы", "да", "для", "до",
                                              "его", "едим", "едят", "ее", "её", "ей", "ел", "ела",
                                              "ем", "ему", "емъ", "если", "ест", "есть", "ешь", "еще",
                                              "ещё", "ею", "же", "за", "и", "из", "или", "им",
                                              "ими", "имъ", "их", "к", "как", "кем", "ко", "когда",
                                              "кого", "ком", "кому", "комья", "которая", "которого",
                                              "которое", "которой", "котором", "которому", "которою",
                                              "которую", "которые", "который", "которым", "которыми",
                                              "которых", "кто", "меня", "мне", "мной", "мною", "мог", "моги",
                                              "могите", "могла", "могли", "могло", "могу", "могут", "мое", "моё",
                                              "моего", "моей", "моем", "моём", "моему", "моею", "можем", "может",
                                              "можете", "можешь", "мои", "мой", "моим", "моими", "моих", "мочь",
                                              "мою", "моя", "мы", "на", "нам", "нами", "нас", "наса",
                                              "наш", "наша", "наше", "нашего", "нашей", "нашем", "нашему", "нашею",
                                              "наши", "нашим", "нашими", "наших", "нашу", "не", "него", "нее",
                                              "неё", "ней", "нем", "нём", "нему", "нет", "нею", "ним",
                                              "ними", "них", "но", "о", "об", "один", "одна", "одни",
                                              "одним", "одними", "одних", "одно", "одного", "одной", "одном", "одному",
                                              "одною", "одну", "он", "она", "оне", "они", "оно", "от",
                                              "по", "при", "с", "сам", "сама", "сами", "самим", "самими",
                                              "самих", "само", "самого", "самом", "самому", "саму", "свое", "своё",
                                              "своего", "своей", "своем", "своём", "своему", "своею", "свои", "свой",
                                              "своим", "своими", "своих", "свою", "своя", "себе", "себя", "собой",
                                              "собою", "та", "так", "такая", "такие", "таким", "такими", "таких",
                                              "такого", "такое", "такой", "таком", "такому", "такою", "такую", "те",
                                              "тебе", "тебя", "тем", "теми", "тех", "то", "тобой", "тобою",
                                              "того", "той", "только", "том", "томах", "тому", "тот", "тою",
                                              "ту", "ты", "у", "уже", "чего", "чем", "чём", "чему",
                                              "что", "чтобы", "эта", "эти", "этим", "этими", "этих", "это",
                                              "этого", "этой", "этом", "этому", "этот", "этою", "эту", "я"};
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
//----------------------------------------------------------------------------------------------------------------------

QString StemmingEng::getStemmedForm(QString &word)
{
    if (word.size() <= 2)
        return word;

    if (special(word))
        return word;

    changeY(word);

    int startR1 = getStartR1(word);
    int startR2 = getStartR2(word, startR1);

    step0(word)
            ;
    if (step1A(word)) {
        std::replace(word.begin(), word.end(), 'Y', 'y');
        return word;
    }

    step1B(word, startR1);
    step1C(word);
    step2(word, startR1);
    step3(word, startR1, startR2);
    step4(word, startR2);
    step5(word, startR1, startR2);

    std::replace(word.begin(), word.end(), 'Y', 'y');
    return word;
}

bool StemmingEng::special(QString &word)
{
    static const QMap<QString, QString> exceptions = {{"skis", "ski"},
                                                      {"skies", "sky"},
                                                      {"dying", "die"},
                                                      {"lying", "lie"},
                                                      {"tying", "tie"},
                                                      {"idly", "idl"},
                                                      {"gently", "gentl"},
                                                      {"ugly", "ugli"},
                                                      {"early", "earli"},
                                                      {"only", "onli"},
                                                      {"singly", "singl"}};

    // special cases
    auto ex = exceptions.find(word);
    if (ex != exceptions.end())
    {
        word = ex.value();
        return true;
    }

    // invariants
    return word.size() >= 3 && word.size() <= 5
           && (word == "sky" || word == "news" || word == "howe"
               || word == "atlas" || word == "cosmos" || word == "bias"
               || word == "andes");
}

void StemmingEng::changeY(QString &word)
{
    if (word[0] == 'y')
        word[0] = 'Y';

    for (int i = 1; i < word.size(); ++i)
    {
        if (word[i] == 'y' && isVowel(word[i - 1]))
            word[i++] = 'Y'; // skip next iteration
    }
}

bool StemmingEng::isVowel(QChar ch)
{
    return ch == 'e' || ch == 'a' || ch == 'i' || ch == 'o' || ch == 'u';
}

bool StemmingEng::isVowelY(QChar ch)
{
    return ch == 'e' || ch == 'a' || ch == 'i' || ch == 'o' || ch == 'u'
           || ch == 'y';
}

int StemmingEng::firstNonVowelAfterVowel(QString &word, int start)
{
    for (int i = start; i != 0 && i < word.size(); ++i)
    {
        if (!isVowelY(word[i]) && isVowelY(word[i - 1]))
            return i + 1;
    }

    return word.size();
}

int StemmingEng::getStartR1(QString &word)
{
    // special cases
    if (word.size() >= 5 && word[0] == 'g' && word[1] == 'e' && word[2] == 'n'
        && word[3] == 'e' && word[4] == 'r')
        return 5;
    if (word.size() >= 6 && word[0] == 'c' && word[1] == 'o' && word[2] == 'm'
        && word[3] == 'm' && word[4] == 'u' && word[5] == 'n')
        return 6;
    if (word.size() >= 5 && word[0] == 'a' && word[1] == 'r' && word[2] == 's'
        && word[3] == 'e' && word[4] == 'n')
        return 5;

    // general case
    return firstNonVowelAfterVowel(word, 1);
}

int StemmingEng::getStartR2(QString &word, int startR1)
{
    if (startR1 == word.size())
        return startR1;

    return firstNonVowelAfterVowel(word, startR1 + 1);
}

bool StemmingEng::replaceIfExists(QString &word, QString suffix, QString replacement, int start)
{
    if (suffix.size() > word.size())
        return false;

    int idx = word.size() - suffix.size();
    if (idx < start)
        return false;

    auto diff = static_cast<std::string::iterator::difference_type>(idx);
    if (std::equal(word.begin() + diff, word.end(), suffix.begin()))
    {
        word.replace(idx, suffix.size(), replacement);
        return true;
    }
    return false;
}

bool StemmingEng::containsVowel(QString &word, int start, int end)
{
    if (end <= word.size())
    {
        for (int i = start; i < end; ++i)
            if (isVowelY(word[i]))
                return true;
    }
    return false;
}

bool StemmingEng::endsInDouble(const QString &word)
{
    if (word.size() >= 2)
    {
        auto a = word[word.size() - 1];
        auto b = word[word.size() - 2];

        if (a == b)
            return a == 'b' || a == 'd' || a == 'f' || a == 'g' || a == 'm'
                   || a == 'n' || a == 'p' || a == 'r' || a == 't';
    }
    return false;
}

bool StemmingEng::isShort(const QString &word)
{
    int size = word.size();

    if (size >= 3)
    {
        if (!isVowelY(word[size - 3]) && isVowelY(word[size - 2])
            && !isVowelY(word[size - 1]) && word[size - 1] != 'w'
            && word[size - 1] != 'x' && word[size - 1] != 'Y')
            return true;
    }
    return size == 2 && isVowelY(word[0]) && !isVowelY(word[1]);
}

bool StemmingEng::isValidLIEnding(QChar ch)
{
    return ch == 'c' || ch == 'd' || ch == 'e' || ch == 'g' || ch == 'h'
           || ch == 'k' || ch == 'm' || ch == 'n' || ch == 'r' || ch == 't';
}

void StemmingEng::step0(QString &word)
{
    // short circuit the longest suffix
    replaceIfExists(word, "'s'", "", 0) ||
            replaceIfExists(word, "'s", "", 0) ||
            replaceIfExists(word, "'", "", 0);
}

bool StemmingEng::step1A(QString &word)
{
    if (!replaceIfExists(word, "sses", "ss", 0))
    {
        if (word.endsWith("ied") || word.endsWith("ies"))
        {
            // if preceded by only one letter
            if (word.size() <= 4)
                word.chop(1);
            else
            {
                word.chop(2);
            }
        }
        else if (word.endsWith("s") && !word.endsWith("us")
                 && !word.endsWith("ss"))
        {
            if (word.size() > 2 && containsVowel(word, 0, word.size() - 2))
                word.chop(1);
        }
    }

    // special case after step 1a
    return (word.size() == 6 || word.size() == 7)
           && (word == "inning" || word == "outing" || word == "canning"
               || word == "herring" || word == "earring" || word == "proceed"
               || word == "exceed" || word == "succeed");
}

void StemmingEng::step1B(QString &word, int startR1)
{
    bool exists = word.endsWith("eedly") || word.endsWith("eed");

    if (exists) // look only in startR1 now
        replaceIfExists(word, "eedly", "ee", startR1)
            || replaceIfExists(word, "eed", "ee", startR1);
    else
    {
        int size = word.size();
        bool deleted = (containsVowel(word, 0, size - 2)
                        && replaceIfExists(word, "ed", "", 0))
                       || (containsVowel(word, 0, size - 4)
                           && replaceIfExists(word, "edly", "", 0))
                       || (containsVowel(word, 0, size - 3)
                           && replaceIfExists(word, "ing", "", 0))
                       || (containsVowel(word, 0, size - 5)
                           && replaceIfExists(word, "ingly", "", 0));

        if (deleted && (word.endsWith("at") ||
                        word.endsWith("bl") ||
                        word.endsWith("iz")))
            word.append('e');
        else if (deleted && endsInDouble(word))
            word.chop(1);
        else if (deleted && startR1 == word.size() && isShort(word))
            word.push_back('e');
    }
}

void StemmingEng::step1C(QString &word)
{
    int size = word.size();
    if (size > 2 && (word[size - 1] == 'y' || word[size - 1] == 'Y'))
        if (!isVowel(word[size - 2]))
            word[size - 1] = 'i';
}

void StemmingEng::step2(QString &word, int startR1)
{
    static const std::pair<QString, QString>
            subs[] = {{"ational", "ate"},
                      {"tional", "tion"},
                      {"enci", "ence"},
                      {"anci", "ance"},
                      {"abli", "able"},
                      {"entli", "ent"},
                      {"izer", "ize"},
                      {"ization", "ize"},
                      {"ation", "ate"},
                      {"ator", "ate"},
                      {"alism", "al"},
                      {"aliti", "al"},
                      {"alli", "al"},
                      {"fulness", "ful"},
                      {"ousli", "ous"},
                      {"ousness", "ous"},
                      {"iveness", "ive"},
                      {"iviti", "ive"},
                      {"biliti", "ble"},
                      {"bli", "ble"},
                      {"fulli", "ful"},
                      {"lessli", "less"}};

    for (auto& sub : subs)
        if (replaceIfExists(word, sub.first, sub.second, startR1))
            return;

    if (!replaceIfExists(word, "logi", "log", startR1 - 1))
    {
        // make sure we choose the longest suffix
        if (word.endsWith("li") && !word.endsWith("abli")
            && !word.endsWith("entli") && !word.endsWith("aliti")
            && !word.endsWith("alli") && !word.endsWith("ousli")
            && !word.endsWith("bli") && !word.endsWith("fulli")
            && !word.endsWith("lessli"))
            if (word.size() > 3 && word.size() - 2 >= startR1
                && isValidLIEnding(word[word.size() - 3]))
            {
                word.chop(2);
            }
    }
}

void StemmingEng::step3(QString &word, int startR1, int startR2)
{
    static const std::pair<QString, QString>
        subs[] = {{"ational", "ate"},
                  {"tional", "tion"},
                  {"alize", "al"},
                  {"icate", "ic"},
                  {"iciti", "ic"},
                  {"ical", "ic"},
                  {"ful", ""},
                  {"ness", ""}};

    for (auto& sub : subs)
        if (replaceIfExists(word, sub.first, sub.second, startR1))
            return;

    replaceIfExists(word, "ative", "", startR2);
}

void StemmingEng::step4(QString &word, int startR2)
{
    static const std::pair<QString, QString>
        subs[] = {{"al", ""},
                  {"ance", ""},
                  {"ence", ""},
                  {"er", ""},
                  {"ic", ""},
                  {"able", ""},
                  {"ible", ""},
                  {"ant", ""},
                  {"ement", ""},
                  {"ment", ""},
                  {"ism", ""},
                  {"ate", ""},
                  {"iti", ""},
                  {"ous", ""},
                  {"ive", ""},
                  {"ize", ""}};

    for (auto& sub : subs)
        if (replaceIfExists(word, sub.first, sub.second, startR2))
            return;

    // make sure we only choose the longest suffix
    if (!word.endsWith("ement") && !word.endsWith("ment"))
        if (replaceIfExists(word, "ent", "", startR2))
            return;

    // short circuit
    replaceIfExists(word, "sion", "s", startR2 - 1)
            || replaceIfExists(word, "tion", "t", startR2 - 1);
}

void StemmingEng::step5(QString &word, int startR1, int startR2)
{
    int size = word.size() - 1;
    if (word[size] == 'e')
    {
        if (size >= startR2)
            word.chop(1);
        else if (size >= startR1 && !isShort(word.left(size)))
            word.chop(1);
    }
    else if (word[word.size() - 1] == 'l')
    {
        if (word.size() - 1 >= startR2 && word[word.size() - 2] == 'l')
            word.chop(1);
    }
}
