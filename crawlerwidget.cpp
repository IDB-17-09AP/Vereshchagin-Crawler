#include "crawlerwidget.h"

CrawlerWidget::CrawlerWidget(QWidget *parent) : QWidget(parent)
{

    edit = new QLineEdit("http://stankin.ru");
    spin = new QSpinBox;
    btn = new QPushButton("Start");
    progress = new QProgressBar;

    spin->setValue(0);

    QHBoxLayout* hor = new QHBoxLayout;
    hor->addWidget(edit);
    hor->addWidget(spin);
    hor->addWidget(btn);
    QVBoxLayout* ver = new QVBoxLayout;
    ver->addLayout(hor);
    ver->addWidget(progress);
    setLayout(ver);

    connect(btn, &QPushButton::clicked, this, &CrawlerWidget::slotClicked);
    connect(&bot, &Crawler::signalProgress, progress, &QProgressBar::setValue);
}

void CrawlerWidget::slotClicked()
{
    try {
        progress->reset();
        bot.setMaximumDepth(spin->value());
        bot.setUrl(edit->text());
        bot.start();
    } catch (std::exception &ex) {
        QMessageBox::critical(this, "Error", QString::fromLatin1(ex.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", "Unknow error");
    }
}
