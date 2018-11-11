#include <QApplication>
#include <QScreen>

#include "browser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Browser browser;
    browser.resize(QGuiApplication::primaryScreen()->size() * 0.7);
    browser.show();

    return a.exec();
}
