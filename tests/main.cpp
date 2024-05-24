#include <repmodel.h>

#include <QAbstractItemModelTester>
#include <QObject>
#include <QTest>

class Tester : public QObject
{
    Q_OBJECT

private slots:
    void testRepModel();
    void testRubric();
};

void Tester::testRepModel()
{
    RepModel model;
    QAbstractItemModelTester tester(&model);
}

void Tester::testRubric()
{
    Q_ASSERT(1 == 1);
}

QTEST_MAIN(Tester)

#include "main.moc"
