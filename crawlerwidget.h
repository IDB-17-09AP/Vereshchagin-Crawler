#ifndef CRAWLERWIDGET_H
#define CRAWLERWIDGET_H

#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QWidget>
#include <QProgressBar>
#include <QMessageBox>

#include <crawler.h>

class CrawlerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CrawlerWidget(QWidget *parent = nullptr);

public slots:
    void slotClicked();

private:
    QLineEdit *edit;
    QPushButton *btn;
    QSpinBox *spin;
    QProgressBar* progress;
    Crawler bot;
};

#endif // CRAWLERWIDGET_H
