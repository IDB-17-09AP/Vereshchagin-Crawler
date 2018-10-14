#include "crawlerwidget.h"

CrawlerWidget::CrawlerWidget(QWidget *parent) : QWidget(parent)
{
    // init
    inputLine = new QLineEdit("http://stankin.ru");
    inputDepth = new QSpinBox;
    startButton = new QPushButton("Start");
    progress = new QProgressBar;
    statusLabel = new QLabel("Ready for work");

    inputDepth->setValue(0);
    inputDepth->setRange(0, 100);

    progress->setRange(0, 100);
    progress->setAlignment(Qt::AlignCenter);

    // layout settings
    QHBoxLayout* hor = new QHBoxLayout;
    hor->addWidget(new QLabel("Link:"));
    hor->addWidget(inputLine);
    hor->addWidget(new QLabel("Depth:"));
    hor->addWidget(inputDepth);
    hor->addWidget(startButton);
    QVBoxLayout* ver = new QVBoxLayout;
    ver->addLayout(hor);
    ver->addWidget(statusLabel);
    ver->addWidget(progress);
    setLayout(ver);

    // connection
    connect(startButton, &QPushButton::clicked, this, &CrawlerWidget::slotClicked);
    connect(&bot, &Crawler::signalProgress, progress, &QProgressBar::setValue);
}

void CrawlerWidget::slotClicked()
{
    try {
        startButton->setEnabled(false);
        statusLabel->setText("Please wait...");
        progress->reset();
        bot.setMaximumDepth(inputDepth->value());
        bot.setUrl(inputLine->text());
        QDesktopServices::openUrl(QUrl::fromLocalFile(bot.start()));
        statusLabel->setText("All ready");
        startButton->setEnabled(true);
    } catch (std::exception &ex) {
        QMessageBox::critical(this, "Error", QString::fromLatin1(ex.what()));
        statusLabel->setText("Error: " + QString::fromLatin1(ex.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", "Unknow error");
        statusLabel->setText("Unknow error");
    }
}
