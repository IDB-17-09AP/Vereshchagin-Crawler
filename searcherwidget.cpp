#include "searcherwidget.h"

SearcherWidget::SearcherWidget(QWidget *parent) : QDialog(parent)
{
    // init
    buttonSelectTxt = new QPushButton("Select txt");
    buttonSelectJson = new QPushButton("Select json");
    buttonSearch = new QPushButton("Search!");
    inputRequest = new QLineEdit;
    searchResult = new QListWidget;

    buttonSearch->setEnabled(false);
    QLabel *tip = new QLabel("?");
    tip->setFixedSize(30, 30);
    tip->setAlignment(Qt::AlignCenter);
    tip->setFont(QFont("Times", 13, 100));
    tip->setStyleSheet("QLabel {"
                       "color: white;"
                       "background: cornflowerblue;"
                       "border: 2px solid white;"
                       "border-radius: 15px; }");
    tip->setToolTip(tr("Инструкция по работе:\n"
                       "Для начала нужно запустить самого crawler'a,\n"
                       "по завершеннию работы которого будет получен txt файл.\n"
                       "Чтоб из него создать поисковый индекс, необходимо загрузить\n"
                       "его нажатием Select txt. Теперь можно пользоваться поиском.\n"
                       "Кроме этого будет создан json файл, в котором будет\n"
                       "находится составленный индекс. В дальнейшем его можно сразу\n"
                       "загружать минуя формирования индекса каждый раз из txt файла.\n\n"
                       "Двойной клик по ответу на запрос открывает его"));

    // layout settings
    QHBoxLayout *upLayout = new QHBoxLayout;
    upLayout->addStretch(1);
    upLayout->addWidget(buttonSelectTxt);
    upLayout->addWidget(buttonSelectJson);
    upLayout->addWidget(tip);

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->addWidget(inputRequest, 5);
    searchLayout->addWidget(buttonSearch, 2);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(upLayout);
    layout->addLayout(searchLayout);
    layout->addWidget(searchResult);

    setLayout(layout);

    setMinimumSize(200, 200);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    // connect
    connect(buttonSelectTxt, &QPushButton::clicked, this, &SearcherWidget::readFromTxt);
    connect(buttonSelectJson, &QPushButton::clicked, this, &SearcherWidget::readFromJson);
    connect(buttonSearch, &QPushButton::clicked, this, &SearcherWidget::search);
    connect(inputRequest, &QLineEdit::returnPressed, this, &SearcherWidget::search);
}

QListWidget *SearcherWidget::resultList()
{
    return searchResult;
}

void SearcherWidget::writeToJson(const Index &index) const
{
    QJsonArray wordArray;

    for (auto wordIt = index.cbegin(); wordIt != index.cend(); ++wordIt) {
        QJsonObject wordObj, indexObj;
        for (auto indexIt = wordIt.value().cbegin(); indexIt != wordIt.value().cend(); ++indexIt) {
            indexObj.insert(indexIt.key(), QJsonValue::fromVariant(indexIt.value()));
            // домен : релевантность
        }
        wordObj.insert(wordIt.key(), indexObj);
        wordArray.append(wordObj);
        // слово : {домен, релевантсность}
    }

    QFile file("test.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(wordArray).toJson(QJsonDocument::Indented));
    }
}

void SearcherWidget::readFromJson()
{
    QString path = QFileDialog::getOpenFileName(this, "Select file", "",
                                                "Json file (*.json) ;; All files (*.*)");
    QFile file(path);

    if (file.open(QIODevice::ReadOnly)) {
        auto mainArray = QJsonDocument::fromJson(file.readAll()).array();
        for (const auto object : mainArray) {
            auto wordObj = object.toObject();
            for (auto i = wordObj.constBegin(); i != wordObj.constEnd(); ++i) {
                auto wordIndex = i.value().toObject();
                for (auto j = wordIndex.constBegin(); j !=wordIndex.constEnd(); ++j) {
                    index[i.key()][j.key()] = j.value().toDouble();
                }
            }
        }
        buttonSearch->setEnabled(true);
    } else {
        qDebug() << "error open file!";
    }
}

void SearcherWidget::search()
{
    QString str = inputRequest->text(), word;
    QVector<QMap<QString, double>* > request;
    QTextStream out(&str);
    while (!out.atEnd()) {
        out >> word;
        word = Stemming::getStemmedForm(word.toLower());
        if (!word.isEmpty())
            request.append(&index[word]);
    }
    if (!request.isEmpty()) {
        QMap<QString, double> preResult;

        QMap<QString, double>::iterator temp;
        for (auto el : request) {
            if (!el->isEmpty()) {
                for (auto it = el->cbegin(); it != el->cend(); ++it)
                    if ((temp = preResult.find(it.key())) != preResult.end())
                        temp.value() += it.value();
                    else
                        preResult.insert(it.key(), it.value());
            }
        }

        std::map<double, QString> result;
        for (auto it = preResult.cbegin(); it != preResult.cend(); ++it)
            result.emplace(it.value(), it.key());

        searchResult->clear();
        for (auto it = result.crbegin(); it != result.crend(); ++it)
            searchResult->addItem(it->second);
    }
}

void SearcherWidget::computeRelevance(int numberDocuments/*,
                                      const QMap<QString, QPair<int, int>> &avgdl,
                                      QMap<QString, int> numberWordInDocs*/)
{
    /*
    for (auto word = index.begin(); word != index.end(); ++word) {
        for (auto domain = word.value().begin(); domain != word.value().end(); ++domain) {
            domain.value() *= (domain.value() + 3) /    // TF + поправка k
                    (domain.value() + 2 *               // TF + поправка b
                    (0.25 + 0.75 * numberWordInDocs[domain.key()]               //  длина документа деленное на
                    / (avgdl[word.key()].first / avgdl[word.key()].second)));   // среднию длину документа коллекции
            // расчёт IDF
            double IDF = qLn((numberDocuments - word.value().size() + 0.5) / (word.value().size() + 0.5));
            domain.value() *= IDF > 0 ? IDF : 0.00001;
        }
    }
    */
    for (auto &word : index) {
        for (auto& value : word) {
                value *= qLn(static_cast<double>(numberDocuments * 100) / word.size());
                // TF-IDF
        }
    }
}

void SearcherWidget::readFromTxt()
{
    auto paths = QFileDialog::getOpenFileNames(this, "Select file", "",
                                                "Text file (*.txt) ;; All files (*.*)");
    int numberDocuments = 0;
    // avgdl - средняя длина документа в коллекции
//    QMap<QString, QPair<int, int>> avgdl;
//    QMap<QString, int> numberWordInDocs;
    for (const auto &path : paths) {
        QFile file(path);

        if (file.open(QIODevice::ReadOnly)) {
            QTextStream fin(&file);
            QString domain, word = "nothing";
            int numberWord, numberWordInDoc;

            while (!fin.atEnd()) {
                fin >> domain;
                fin >> numberWordInDoc;
                ++numberDocuments;

//                numberWordInDocs[domain] = numberWordInDoc;

                while (!word.isEmpty()) {
                    fin >> word;
                    if (word == "<endLine>")
                        break;

                    fin >> numberWord;
                    if (numberWordInDoc) {  // вычисление TF
                        index[word][domain] = 1 + static_cast<double>(numberWord) / numberWordInDoc;
//                        avgdl[word].first += numberWordInDoc;
//                        ++avgdl[word].second;
                    }
                }
            }
        } else {
            QMessageBox::critical(this, tr("Error!"), tr("Error open file!"));
        }
    }
    computeRelevance(numberDocuments/*, avgdl, numberWordInDocs*/);

    writeToJson(index);
    buttonSearch->setEnabled(true);
}
