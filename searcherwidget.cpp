#include "searcherwidget.h"

SearcherWidget::SearcherWidget(QWidget *parent) : QWidget(parent)
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
                       "загружать минуя формирования индекса каждый раз из txt файла."));

    // layout settings
    QHBoxLayout *upLayout = new QHBoxLayout;
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

    // connect
    connect(buttonSelectTxt, &QPushButton::clicked, this, &SearcherWidget::readFromTxt);
    connect(buttonSelectJson, &QPushButton::clicked, this, &SearcherWidget::readFromJson);
    connect(buttonSearch, &QPushButton::clicked, this, &SearcherWidget::search);
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
    QVector<QString> request;
    QTextStream out(&str);
    while (!out.atEnd()) {
        out >> word;
        word = Stemming::getStemmedForm(word);
        if (!word.isEmpty())
            request.append(word);
    }
    if (!request.isEmpty()) {
        QMap<QString, double> result;
        auto &collectionFirst = index[request[0]];
        for (int i = 1; i < request.size(); ++i) {
            auto &collectionSecond = index[request[i]];
            for (auto it = collectionSecond.cbegin(); it != collectionSecond.cend(); ++it) {
                if (i == 1) {
                    if (collectionFirst.contains(it.key()))
                        result.insert(it.key(), it.value());
                } else {
                    if (result.contains(it.key()))
                        result.insert(it.key(), it.value());
                }
            }
        }
        if (result.isEmpty())
            result = collectionFirst;
        searchResult->clear();
        for (auto it = result.cbegin(); it != result.cend(); ++it) {
            searchResult->addItem(it.key());
        }
    }
}

void SearcherWidget::computeRelevance(QMap<QString, double> &data, int numberDocuments)
{
    for (auto& value : data) {
        value *= qLn(static_cast<double>(numberDocuments) / data.size());
        // TF-IDF
    }
}

void SearcherWidget::readFromTxt()
{
    QString path = QFileDialog::getOpenFileName(this, "Select file", "",
                                                "Text file (*.txt) ;; All files (*.*)");
    QFile file(path);

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream fin(&file);
        QString domain, word = "nothing";
        int numberWord, numberWordInDoc, numberDocuments = 0;

        while (!fin.atEnd()) {
            fin >> domain;
            fin >> numberWordInDoc;
            ++numberDocuments;
            while (!word.isEmpty()) {
                fin >> word;
                if (word == "<endLine>")
                    break;

                fin >> numberWord;
                if (numberWordInDoc) {  // вычисление TF
                    index[word][domain] = 1 + static_cast<double>(numberWord) / numberWordInDoc;
                }
            }
        }

        for (auto &word : index)
            computeRelevance(word, numberDocuments * 100);

        writeToJson(index);
        buttonSearch->setEnabled(true);
    } else {
        qDebug() << "error open file!";
    }

}