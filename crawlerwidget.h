#ifndef CRAWLERWIDGET_H
#define CRAWLERWIDGET_H

#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QProgressBar>
#include <QMessageBox>
#include <QDesktopServices>

#include <crawler.h>

class CrawlerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CrawlerWidget(QWidget *parent = nullptr);

public slots:
    void slotClicked();

private:
    QLineEdit *inputLine;
    QLabel *statusLabel;
    QPushButton *startButton;
    QSpinBox *inputDepth;
    QProgressBar* progress;
    Crawler bot;
};

#endif // CRAWLERWIDGET_H
