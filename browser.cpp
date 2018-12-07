#include "browser.h"
#include <QWebEngineSettings>

Browser::Browser(QWidget *parent) : QMainWindow (parent)
{
    // init
    lineUrl = new QLineEdit("http://stankin.ru");
    webView = new QWebEngineView;
    buttonBack = new QPushButton;
    buttonForward = new QPushButton;
    buttonRefreshAndStop = new QPushButton;
    searcher = new SearcherWidget(this);
    refresh = true;

    QProgressBar *progressLoad = new QProgressBar;
    QPushButton *buttonGo = new QPushButton();
    QPushButton *buttonHome = new QPushButton();

    setWindowIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));

    // settings
    buttonRefreshAndStop->setIcon(QIcon(QStringLiteral(":/refresh.png")));
    buttonRefreshAndStop->setIconSize(QSize(20, 20));
    buttonForward->setEnabled(false);
    buttonForward->setIcon(QIcon(QStringLiteral(":/forward.png")));
    buttonForward->setIconSize(QSize(20, 20));
    buttonBack->setEnabled(false);
    buttonBack->setIcon(QIcon(QStringLiteral(":/back.png")));
    buttonBack->setIconSize(QSize(20, 20));
    buttonHome->setIcon(QIcon(QStringLiteral(":/home.png")));
    buttonHome->setIconSize(QSize(20, 20));
    buttonGo->setIcon(QIcon(QStringLiteral(":/search.png")));
    buttonGo->setIconSize(QSize(20, 20));

    lineUrl->setFixedHeight(26);
    lineUrl->setFont(QFont("Segoe UI", 12));

    searcher->resize(480, 320);

    QMenu *menuFile = new QMenu(tr("&File"));
    menuFile->addSeparator();
    menuFile->addAction(tr("&Quit"), this, &Browser::close, QKeySequence("CTRL+Q"));
    menuBar()->addMenu(menuFile);

    QMenu *menuCrawler = new QMenu(tr("&Crawler"));
    menuCrawler->addAction(tr("Start crawler"), [this](){
        Crawler crawler(this);
        crawler.exec();
    });
    menuCrawler->addAction(tr("Folder with files"), [](){
        QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath()));
    });
    menuBar()->addMenu(menuCrawler);

    QMenu *menuSearcher = new QMenu(tr("&Searcher"));
    menuSearcher->addAction(tr("Start searcher"), this, &Browser::slotHome);
    menuBar()->addMenu(menuSearcher);

    QMenu *menuHelp = new QMenu(tr("&Help"));
//    menuHelp->addAction(tr("&Settings"), [this](){
//        Settings settingsWindow(this);
//        settingsWindow.exec();
//    });
    menuHelp->addSeparator();
    menuHelp->addAction(tr("&About"), [this](){
        QMessageBox::about(this, tr("About"), tr("Create by:\n"
                                                 "Nikolay Vereshchagin\n"
                                                 "IDB-17-09"));
    }, QKeySequence("CTRL+ALT+A"));
    menuBar()->addMenu(menuHelp);

    statusBar()->addWidget(progressLoad);

    setMinimumSize(640, 480);

    // layout setup
    QHBoxLayout *upLayout = new QHBoxLayout;
    upLayout->addWidget(buttonBack);
    upLayout->addWidget(buttonRefreshAndStop);
    upLayout->addWidget(buttonForward);
    upLayout->addWidget(buttonHome);
    upLayout->addWidget(lineUrl, 10);
    upLayout->addWidget(buttonGo);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(upLayout);
    layout->addWidget(webView);

    QWidget *wgt = new QWidget;
    wgt->setLayout(layout);
    setCentralWidget(wgt);

    // connection
    connect(buttonGo, &QPushButton::clicked, this, &Browser::slotGo);
    connect(lineUrl, &QLineEdit::returnPressed, this, &Browser::slotGo);
    connect(buttonBack, &QPushButton::clicked, webView, &QWebEngineView::back);
    connect(buttonForward, &QPushButton::clicked, webView, &QWebEngineView::forward);
    connect(buttonRefreshAndStop, &QPushButton::clicked, this, &Browser::slotButtonRefreshAndStop);
    connect(buttonHome, &QPushButton::clicked, this, &Browser::slotHome);
    connect(webView, &QWebEngineView::loadStarted, this, &Browser::slotSetStopButton);
    connect(webView, &QWebEngineView::loadProgress, progressLoad, &QProgressBar::setValue);
    connect(webView, &QWebEngineView::loadFinished, this, &Browser::slotFinished);
    connect(searcher->resultList(), &QListWidget::itemDoubleClicked, this, &Browser::slotGoForSearcher);

    slotGo();
}

void Browser::slotGo()
{
    if (!lineUrl->text().startsWith("ftp://")
            && !lineUrl->text().startsWith("http://")
            && !lineUrl->text().startsWith("gopher://")) {
        webView->load(QUrl("http://" + lineUrl->text()));
    } else {
        webView->load(QUrl(lineUrl->text()));
    }
}

void Browser::slotGoForSearcher(QListWidgetItem *item)
{
    lineUrl->setText(item->text());
    webView->load(item->text());
    searcher->close();
}

void Browser::slotSetStopButton()
{
    buttonRefreshAndStop->setIcon(QIcon(QStringLiteral(":/stop.png")));
    refresh = false;
}

void Browser::slotButtonRefreshAndStop()
{
    if (refresh) {
        webView->reload();
    } else {
        webView->stop();
    }
}

void Browser::slotFinished(bool status)
{
    if (!status) {
        webView->setHtml("<center>An error has occured"
                         " while loading the web page</center>");
    } else {
       lineUrl->setText(webView->url().toString());
    }
    buttonRefreshAndStop->setIcon(QIcon(QStringLiteral(":/refresh.png")));
    refresh = true;
    buttonBack->setEnabled(webView->history()->canGoBack());
    buttonForward->setEnabled(webView->history()->canGoForward());
}

void Browser::slotHome()
{
    searcher->exec();
}


void Browser::closeEvent(QCloseEvent *event)
{
    webView->close();
    QMainWindow::closeEvent(event);
}
