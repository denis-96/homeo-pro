#include <QtTest>
#include <rubric.h>
// add necessary includes here

class TestRubric : public QObject
{
    Q_OBJECT

public:
    TestRubric();
    ~TestRubric();

private slots:
    // void initTestCase();
    // void cleanupTestCase();
    void testTitle();
    void testImportance();
    void testIsValid();
    void testDrugs();
    void testSubrubrics();
};

TestRubric::TestRubric() {}

TestRubric::~TestRubric() {}

// void TestRubric::initTestCase() {}

// void TestRubric::cleanupTestCase() {}

void TestRubric::testTitle()
{
    Rubric rubric("rubric title");
    QCOMPARE(rubric.title(), QString("rubric title"));

    rubric.setTitle("new title");
    QCOMPARE(rubric.title(), QString("new title"));

    rubric.setTitle("");
    QCOMPARE_NE(rubric.title(), QString(""));
}

void TestRubric::testImportance()
{
    Rubric r;
    QCOMPARE(r.importance(), 1);

    r.setImportance(0);
    QCOMPARE(r.importance(), 0);
    r.setImportance(4);
    QCOMPARE(r.importance(), 4);
}

void TestRubric::testIsValid()
{
    QVERIFY(!Rubric().isValid());
    QVERIFY(Rubric("some title").isValid());
}

void TestRubric::testDrugs()
{
    Rubric r;
    QString d("drug");
    QString d2("drug 2");
    QCOMPARE(r.drugCount(), 0);
    QCOMPARE(r.drugDegree(d), 0);
    QVERIFY(!r.removeDrug(d));

    r.addDrug(d, 1);
    QCOMPARE(r.drugCount(), 1);
    QCOMPARE(r.drugDegree(d), 1);
    r.addDrug(d, 2);
    QCOMPARE(r.drugCount(), 1);
    QCOMPARE(r.drugDegree(d), 2);

    r.addDrug(d2, 3);
    QCOMPARE(r.drugCount(), 2);
    QCOMPARE(r.drugDegree(d2), 3);

    r.removeDrug(d);
    QCOMPARE(r.drugCount(), 1);
}

void TestRubric::testSubrubrics() {}

QTEST_APPLESS_MAIN(TestRubric)

#include "tst_testrubric.moc"
