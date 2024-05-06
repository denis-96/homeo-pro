#include "repmodel.h"

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
    QAbstractItemModelTester tester(&model);
}

QTEST_APPLESS_MAIN(TestRepModel)

#include "test.moc"
