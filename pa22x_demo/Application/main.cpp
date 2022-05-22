#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator trans;
    if (trans.load("Application_zh.qm", ":/"))
    {
        a.installTranslator(&trans);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
