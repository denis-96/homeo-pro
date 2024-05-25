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
    void testConvertion_data();
    void testConvertion();
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

void TestRubric::testConvertion_data()
{
    using result_t = std::pair<QString, Rubric::Drugs>;
    QTest::addColumn<QString>("before");
    QTest::addColumn<result_t>("parsed");
    QTest::addColumn<QString>("after");

    result_t result1("Горло; боль; справа; ночь, ночью",
                     {{"aq-mar", 2}, {"crot-c", 1}, {"gink", 1}, {"lac-c", 4}, {"sabad", 3}});
    result_t result2("Горло; боль; справа; ночь, ночью", {});

    QTest::newRow("standart") << "Горло; боль; справа; ночь, ночью: "
                                 "aq-mar(2) crot-c gink LAC-C(4) SABAD(3)."
                              << result1
                              << "Горло; боль; справа; ночь, ночью: "
                                 "aq-mar(2) crot-c gink LAC-C(4) SABAD(3).";
    QTest::newRow("without dot") << "Горло; боль; справа; ночь, ночью: "
                                    "aq-mar(2) crot-c gink LAC-C(4) SABAD(3)"
                                 << result1
                                 << "Горло; боль; справа; ночь, ночью: "
                                    "aq-mar(2) crot-c gink LAC-C(4) SABAD(3).";
    QTest::newRow("without drugs 1")
        << "Горло; боль; справа; ночь, ночью: " << result2 << "Горло; боль; справа; ночь, ночью.";
    QTest::newRow("without drugs 2")
        << "Горло; боль; справа; ночь, ночью" << result2 << "Горло; боль; справа; ночь, ночью.";
}

void TestRubric::testConvertion()
{
    using result_t = std::pair<QString, Rubric::Drugs>;
    QFETCH(QString, before);
    QFETCH(result_t, parsed);
    QFETCH(QString, after);

    auto r = Rubric::fromString(before);
    QCOMPARE(r->title(), parsed.first);
    QCOMPARE((Rubric::Drugs::size_type) r->drugCount(), parsed.second.size());

    for (auto const &d : parsed.second) {
        QCOMPARE(r->drugDegree(d.first), d.second);
    }

    QCOMPARE(r->toString(), after);
}

void TestRubric::testSubrubrics()
{
    // TODO: write test for subrubrics functionality
    auto r = new Rubric;

    QCOMPARE(r->parentRubric(), nullptr);
    QCOMPARE(r->subrubricCount(), 0);
    QCOMPARE(r->subrubric(12), nullptr);

    r->addSubrubric(std::make_unique<Rubric>("sub", Rubric::Drugs{{"d1", 2}, {"d2", 1}}));

    QCOMPARE(r->subrubricCount(), 1);
    QCOMPARE(r->drugCount(), 2);
    QVERIFY(r->subrubric(1)->parentRubric());

    r->addSubrubric(std::make_unique<Rubric>("sub", Rubric::Drugs{{"d2", 4}, {"d3", 1}}));

    QCOMPARE(r->subrubricCount(), 2);
    QCOMPARE(r->drugCount(), 3);
    QCOMPARE(r->drugDegree("d2"), 4);
}

QTEST_APPLESS_MAIN(TestRubric)

#include "tst_testrubric.moc"
