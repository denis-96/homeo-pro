#include "HomeoPro.h"

#include <QApplication>
#include <QFile>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    HomePro window;
    QTimer::singleShot(0, &window, &HomePro::openWith);

    window.show();

    return application.exec();
}
