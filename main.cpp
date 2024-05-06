#include "repertorizator.h"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    Repertorizator window;
    QTimer::singleShot(0, &window, SLOT(initialize()));

    window.show();

    return application.exec();
}
