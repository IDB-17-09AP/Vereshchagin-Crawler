#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>

#include "crawlerwidget.h"
#include "searcherwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget wgt;
    CrawlerWidget *crawler = new CrawlerWidget;
    SearcherWidget *searcher = new SearcherWidget;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(crawler);
    layout->addWidget(searcher);
    wgt.setLayout(layout);

    wgt.show();

    return a.exec();
}
