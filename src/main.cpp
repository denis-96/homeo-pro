#include <repertorizator.h>

#include <QApplication>
#include <QFile>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    Repertorizator window;
    QTimer::singleShot(0, &window, &Repertorizator::openWith);

    window.show();

    return application.exec();
}
