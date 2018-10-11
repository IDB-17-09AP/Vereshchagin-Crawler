#include <QApplication>

#include "crawlerwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CrawlerWidget wgt;
    wgt.setMinimumSize(350, 100);
    wgt.show();

    return a.exec();
}
