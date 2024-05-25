#include "rubric.h"
#include <QDebug>
#include <set>

Rubric::Rubric(const QString &title, const Drugs &drugs, Rubric *parentRubric)
    : _title(title)
    , _drugs(drugs)
    , _parentRubric(parentRubric)
{}

Rubric::~Rubric() {}

void Rubric::setTitle(const QString &newTitle)
{
    if (!newTitle.isEmpty())
        _title = newTitle;
}
void Rubric::setImportance(unsigned short newImportance)
{
    _importance = newImportance;
}
unsigned char Rubric::drugDegree(const QString &drug) const
{
    auto drugIt = _drugs.find(drug);
    if (drugIt != _drugs.end())
        return drugIt->second;

    return 0;
}
bool Rubric::addDrug(const QString &drug, unsigned char degree)
{
    if (degree > 4)
        return false;

    _drugs[drug] = degree;
    return true;
}

bool Rubric::removeDrug(const QString &drug)
{
    return _drugs.erase(drug);
}

Rubric *Rubric::subrubric(int number)
{
    if (number < subrubricCount())
        return _subrubrics[number].get();
    return nullptr;
}

void Rubric::addSubrubric(std::unique_ptr<Rubric> &&rubric)
{
    for (const auto &pair : rubric->_drugs) {
        auto drug = pair.first;
        auto degree = pair.second;
        if (drugDegree(drug) < degree) {
            addDrug(drug, degree);
        }
    }

    rubric->_parentRubric = this;
    _subrubrics.push_back(std::move(rubric));
}

std::unique_ptr<Rubric> Rubric::removeSubrubric(int number)
{
    if (number >= subrubricCount())
        return {};

    auto rubric = subrubric(number);
    auto rubricIt = _subrubrics.cbegin() + number;

    for (const auto &pair : rubric->_drugs) {
        auto drug = pair.first;

        unsigned short maxDegree = 0;
        for (const auto &subRub : _subrubrics)
            if (subRub.get() != rubric)
                maxDegree = std::max<unsigned short>(subRub->drugDegree(drug), maxDegree);

        if (maxDegree == 0)
            removeDrug(drug);
        else
            addDrug(drug, maxDegree);
    }
    rubric->_parentRubric = nullptr;

    auto rubricIdx = std::distance(_subrubrics.cbegin(), rubricIt);
    std::unique_ptr<Rubric> rubricPtr = std::move(_subrubrics[rubricIdx]);

    _subrubrics.erase(rubricIt);

    return rubricPtr;
}

std::unique_ptr<Rubric> Rubric::fromString(const QString &str)
{
    QStringList splitedRubric = str.split(": ");
    if (splitedRubric.size() == 1)
        splitedRubric.push_back("");

    auto rubric = std::make_unique<Rubric>(splitedRubric[0]);

    if (splitedRubric[1].endsWith('.'))
        splitedRubric[1].chop(1);

    QStringList unparsedDrugs = splitedRubric[1].split(' ', Qt::SkipEmptyParts);

    for (auto &drug : unparsedDrugs) {
        QStringList splitedDrug = drug.split("(");
        if (splitedDrug.size() > 2)
            continue;

        QString drugTitle = splitedDrug[0].toLower();
        quint8 drugDegree = 1;
        if (splitedDrug.size() == 2) {
            bool ok;
            drugDegree = splitedDrug[1].chopped(1).toUShort(&ok);
            if (!ok)
                continue;
        }

        rubric->addDrug(drugTitle, drugDegree);
    }
    return rubric;
}

QString Rubric::toString()
{
    if (!isValid())
        return {};

    QString rubStr(title());
    if (drugCount()) {
        rubStr.push_back(':');

        for (const auto &drug :
             std::set<decltype(_drugs)::value_type>(_drugs.cbegin(), _drugs.cend())) {
            rubStr.push_back(' ');
            rubStr.push_back(drug.second > 2 ? drug.first.toUpper() : drug.first);
            if (drug.second > 1)
                rubStr.push_back('(' + QString::number(drug.second) + ')');
        }
    }

    rubStr.push_back('.');

    return rubStr;
}
