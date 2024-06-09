#include <QtTest>
#include <Rubric.h>
// add necessary includes here

class TestRubric : public QObject
{
    Q_OBJECT

public:
    TestRubric();
    ~TestRubric();

private:
    struct RubricData
    {
        QString title;
        Rubric::Drugs drugs;
    };

private slots:
    // void initTestCase();
    // void cleanupTestCase();
    void testTitle();
    void testImportance();
    void testIsValid();
    void testDrugs();
    void testConvertion_data();
    void testConvertion();
    void testSubrubrics_data();
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
    Rubric r1;
    QCOMPARE(r1.drugCount(), 0);
    QCOMPARE(r1.drugDegree("d1"), 0);

    Rubric r2("title", {{"d1", 2}, {"d2", 4}});
    QCOMPARE(r2.drugCount(), 2);
    QCOMPARE(r2.drugDegree("d1"), 2);
    QCOMPARE(r2.drugDegree("d2"), 4);
    QCOMPARE(r2.drugDegree("d3"), 0);
}

void TestRubric::testConvertion_data()
{
    QTest::addColumn<QString>("before");
    QTest::addColumn<RubricData>("parsed");
    QTest::addColumn<QString>("after");

    QString baseStr("Горло; боль; справа; ночь, ночью: "
                    "aq-mar(2) crot-c gink LAC-C(4) SABAD(3).");
    QStringList splitedBaseStr = baseStr.split(':');
    RubricData result1{"Горло; боль; справа; ночь, ночью",
                       {{"aq-mar", 2}, {"crot-c", 1}, {"gink", 1}, {"lac-c", 4}, {"sabad", 3}}};
    RubricData result2{"Горло; боль; справа; ночь, ночью", {}};

    QTest::newRow("normal") << baseStr << result1 << baseStr;
    QTest::newRow("without dot") << baseStr.chopped(1) << result1 << baseStr;
    QTest::newRow("without drugs 1")
        << splitedBaseStr[0] + ": " << result2 << splitedBaseStr[0] + '.';
    QTest::newRow("without drugs 2") << splitedBaseStr[0] << result2 << splitedBaseStr[0] + '.';
}

void TestRubric::testConvertion()
{
    QFETCH(QString, before);
    QFETCH(RubricData, parsed);
    QFETCH(QString, after);

    auto r = Rubric::fromString(before);
    QCOMPARE(r->title(), parsed.title);
    QCOMPARE((Rubric::Drugs::size_type) r->drugCount(), parsed.drugs.size());

    for (auto const &d : parsed.drugs) {
        QCOMPARE(r->drugDegree(d.first), d.second);
    }

    QCOMPARE(r->toString(), after);
}

void TestRubric::testSubrubrics_data()
{
    QTest::addColumn<QList<RubricData>>("inputRubrics");
    QTest::addColumn<QList<Rubric::Drugs>>("expectedState");

    RubricData r1{"rub1", {{"d1", 2}, {"d2", 1}, {"d3", 4}}};
    RubricData r2{"rub2", {{"d4", 1}, {"d5", 3}, {"d6", 4}}};
    RubricData r3{"rub3", {{"d2", 3}, {"d3", 1}}};
    RubricData r4{"rub4", {{"d1", 3}, {"d2", 1}, {"d5", 1}}};

    QList<RubricData> inputRubrics{r1};
    QList<Rubric::Drugs> expectedState{r1.drugs};

    QTest::newRow("one rubric") << inputRubrics << expectedState;

    inputRubrics = {r1, r2};
    Rubric::Drugs r12(r1.drugs);
    r12.insert(r2.drugs.cbegin(), r2.drugs.cend());
    expectedState = {r1.drugs, r12};
    QTest::newRow("different drugs") << inputRubrics << expectedState;

    inputRubrics = {r1, r3};
    Rubric::Drugs r13{{"d1", 2}, {"d2", 3}, {"d3", 4}};
    expectedState = {r1.drugs, r13};
    QTest::newRow("same drugs") << inputRubrics << expectedState;

    inputRubrics = {r1, r4};
    expectedState = {r1.drugs, {{"d1", 3}, {"d2", 1}, {"d3", 4}, {"d5", 1}}};
    QTest::newRow("mixed drugs") << inputRubrics << expectedState;

    inputRubrics = {r1, r2, r3, r4};
    Rubric::Drugs r123(r13);
    r123.insert(r2.drugs.cbegin(), r2.drugs.cend());
    expectedState = {r1.drugs,
                     r12,
                     r123,
                     {{"d1", 3}, {"d2", 3}, {"d3", 4}, {"d4", 1}, {"d5", 3}, {"d6", 4}}};
    QTest::newRow("many subrubrics") << inputRubrics << expectedState;
}

void TestRubric::testSubrubrics()
{
    QFETCH(QList<RubricData>, inputRubrics);
    QFETCH(QList<Rubric::Drugs>, expectedState);

    Rubric r;
    auto subrubCount = r.subrubricCount();

    QCOMPARE(subrubCount, 0);

    for (const auto &subrub : inputRubrics) {
        auto idx = r.addSubrubric(std::make_unique<Rubric>(subrub.title, subrub.drugs));
        QCOMPARE_NE(idx, -1);
        QCOMPARE(r.subrubric(idx)->title(), subrub.title);

        QCOMPARE((Rubric::Drugs::size_type) r.drugCount(), expectedState[subrubCount].size());
        for (const auto &d : expectedState[subrubCount]) {
            QCOMPARE(r.drugDegree(d.first), d.second);
        }

        QCOMPARE(r.subrubricCount(), ++subrubCount);
    }

    for (int i = r.subrubricCount() - 1; i > 0; --i) {
        r.removeSubrubric(i);

        QCOMPARE((Rubric::Drugs::size_type) r.drugCount(), expectedState[i - 1].size());
        for (const auto &d : expectedState[i - 1]) {
            QCOMPARE(r.drugDegree(d.first), d.second);
        }

        QCOMPARE(r.subrubricCount(), i);
    }
    r.removeSubrubric(0);
    QCOMPARE(r.drugCount(), 0);
    QCOMPARE(r.subrubricCount(), 0);
}

QTEST_APPLESS_MAIN(TestRubric)

#include "tst_rubric.moc"
