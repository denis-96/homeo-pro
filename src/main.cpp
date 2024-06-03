#include <repertorizator.h>

#include <QApplication>
#include <QFile>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    Repertorizator window;
    QTimer::singleShot(0, &window, [&application, &window] {
        QStringList arguments = application.arguments();
#ifdef DEBUG
        arguments.push_back(":/example.rep");
#endif
        if (arguments.size() > 1 && QFile::exists(arguments.last())) {
            auto fileName = arguments.last();
            window.readRep(fileName);
        }
    });

    window.show();

    return application.exec();
}
