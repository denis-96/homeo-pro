#include "../src/repmodel.h"

#include <QAbstractItemModelTester>
#include <QObject>
#include <QTest>

class TestRepModel : public QObject
{
    Q_OBJECT

private slots:
    void testRepModel();
};

void TestRepModel::testRepModel()
{
    RepModel model;
    QTest::qExec(this);
    QAbstractItemModelTester tester(&model);
}

QTEST_MAIN(TestRepModel)

#include "test_repmodel.moc"
