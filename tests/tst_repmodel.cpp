#include <QtTest>
#include <repmodel.h>

class TestRepModel : public QObject
{
    Q_OBJECT

public:
    TestRepModel();
    ~TestRepModel();

private:
    QStringList rubrics{"rub1: d1(2) d2(1) d3(4).",
                        "rub2: d4(1) d5(3) d6(4).",
                        "rub3: d2(3) d3(1).",
                        "rub4: d1(3) d2(1) d5(1)."};

private slots:
    void testInterface();
    void testAddRemoveRubric_data();
    void testAddRemoveRubric();
};

TestRepModel::TestRepModel() {}

TestRepModel::~TestRepModel() {}

void TestRepModel::testInterface()
{
    RepModel model;
    QAbstractItemModelTester tester(&model);
}

void TestRepModel::testAddRemoveRubric_data()
{
    QTest::addColumn<QStringList>("inputRubrics");
    QTest::addColumn<QList<QStringList>>("expectedHData");
    QTest::addColumn<QList<QStringList>>("expectedData");

    QTest::newRow("one rubric") << QStringList{rubrics[0]} << QList<QStringList>{{"d3", "d1", "d2"}}
                                << QList<QStringList>{{"rub1", "4", "2", "1"}};

    QTest::newRow("many rubrics") << QStringList(rubrics)
                                  << QList<QStringList>{{"d3", "d1", "d2"},
                                                        {"d3", "d6", "d5", "d1", "d2", "d4"},
                                                        {"d3", "d2", "d6", "d5", "d1", "d4"},
                                                        {"d2", "d1", "d3", "d5", "d6", "d4"}}
                                  << QList<QStringList>{{"rub1", "4", "2", "1"},
                                                        {"rub2", "", "4", "3", "", "", "1"},
                                                        {"rub3", "1", "3", "", "", "", ""},
                                                        {"rub4", "1", "3", "", "1", "", ""}};
}

void TestRepModel::testAddRemoveRubric()
{
    QFETCH(QList<QString>, inputRubrics);
    QFETCH(QList<QStringList>, expectedHData);
    QFETCH(QList<QStringList>, expectedData);

    RepModel model;
    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.columnCount(), 1);

    auto prevRowCount = model.rowCount();
    for (const auto &r : inputRubrics) {
        auto idx = model.addRubric(r);

        QVERIFY(idx.isValid());
        QCOMPARE(model.columnCount(), expectedHData[prevRowCount].size() + 1);
        for (int c = 1; c < model.columnCount(); ++c) {
            qDebug() << c;
            QCOMPARE(model.headerData(c, Qt::Horizontal, Qt::UserRole).toString(),
                     expectedHData[prevRowCount][c - 1]);
        }
        for (int c = 0; c < model.columnCount(); ++c) {
            QCOMPARE(model.data(model.index(idx.row(), c)).toString(),
                     expectedData[prevRowCount][c]);
        }
        QCOMPARE(model.rowCount(), ++prevRowCount);
    }

    for (int i = model.rowCount() - 1; i > 0; --i) {
        model.removeRubric(model.index(i, 0));
        QCOMPARE(model.rowCount(), i);
        QCOMPARE(model.columnCount(), expectedHData[i - 1].size() + 1);
        for (int c = 1; c < model.columnCount(); ++c) {
            QCOMPARE(model.headerData(c, Qt::Horizontal, Qt::UserRole).toString(),
                     expectedHData[i - 1][c - 1]);
        }
    }
    model.removeRubric(model.index(0, 0));
    QCOMPARE(model.columnCount(), 1);
    QCOMPARE(model.rowCount(), 0);
}

QTEST_APPLESS_MAIN(TestRepModel)

#include "tst_repmodel.moc"
