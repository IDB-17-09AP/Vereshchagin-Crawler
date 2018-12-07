#ifndef SEARCHERWIDGET_H
#define SEARCHERWIDGET_H

#include <QtWidgets>

#include "stemming.h"

using Index = QMap<QString, QMap<QString, double>>;

class SearcherWidget : public QDialog
{
    Q_OBJECT
public:
    SearcherWidget(QWidget *parent = nullptr);
    QListWidget *resultList();

public slots:
    void writeToJson(const Index &index) const;
    void readFromJson();
    void readFromTxt();
    void search();

private:
    void computeRelevance(int numberDocuments/*,
                          const QMap<QString, QPair<int, int> > &avgdl,
                          QMap<QString, int> numberWordInDocs*/);

    QPushButton *buttonSelectTxt;
    QPushButton *buttonSelectJson;
    QPushButton *buttonSearch;
    QLineEdit *inputRequest;
    QListWidget *searchResult;
    Index index;
};

#endif // SEARCHERWIDGET_H
