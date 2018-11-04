#ifndef SEARCHERWIDGET_H
#define SEARCHERWIDGET_H

#include <QWidget>
#include <QtCore/qmath.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QPair>
#include <QMap>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QDebug>

#include "stemming.h"

using Index = QMap<QString, QMap<QString, double>>;

class SearcherWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearcherWidget(QWidget *parent = nullptr);

public slots:
    void writeToJson(const Index &index) const;
    void readFromJson();
    void readFromTxt();
    void search();

private:
    void computeRelevance(QMap<QString, double> &data, int numberDocuments);

    QPushButton *buttonSelectTxt;
    QPushButton *buttonSelectJson;
    QPushButton *buttonSearch;
    QLineEdit *inputRequest;
    QListWidget *searchResult;
    Index index;
};

#endif // SEARCHERWIDGET_H
