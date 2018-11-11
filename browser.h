#ifndef BROWSER_H
#define BROWSER_H

#include <QtWidgets>
#include <QWebEngineView>
#include <QWebEngineHistory>

#include "crawler.h"
#include "searcherwidget.h"

class Browser : public QMainWindow
{
    Q_OBJECT
public:
    Browser(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

protected slots:
    void slotGo();
    void slotGoForSearcher(QListWidgetItem *item);
    void slotSetStopButton();
    void slotButtonRefreshAndStop();
    void slotFinished(bool status);
    void slotHome();

private:
    QLineEdit *lineUrl;
    QWebEngineView *webView;
    SearcherWidget *searcher;
    QPushButton *buttonBack;
    QPushButton *buttonForward;
    QPushButton *buttonRefreshAndStop;
    bool refresh;
};



#endif // BROWSER_H
