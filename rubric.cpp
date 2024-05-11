#include "rubric.h"
#include <QDebug>

Rubric::Rubric(const QString &title, const Drugs &drugs, Rubric *parentRubric)
    : _title(title)
    , _drugs(drugs)
    , _parentRubric(parentRubric)
{
    qDebug() << "Rubric with title" << _title << "created.";
}

Rubric::~Rubric()
{
    for (const auto &drug : _drugs) {
        drug.first->removeRubric(this);
    }
    qDebug() << "Rubric with title" << _title << "deleted.";
}

unsigned char Rubric::drugDegree(Drug *drug) const
{
    auto drugIt = _drugs.find(drug);
    if (drugIt != _drugs.end())
        return drugIt->second;

    return 0;
}
void Rubric::addDrug(Drug *drug, unsigned char degree)
{
    if (degree <= 4)
        _drugs[drug] = degree;
}

void Rubric::removeDrug(Drug *drug)
{
    _drugs.erase(drug);
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
        Drug *drug = pair.first;
        auto degree = pair.second;
        if (drugDegree(drug) < degree) {
            addDrug(drug, degree);
            drug->addRubric(this, degree);
        }
        drug->removeRubric(rubric.get());
    }
    _subrubrics.push_back(std::move(rubric));
}

std::unique_ptr<Rubric> Rubric::removeSubrubric(Rubric *rubric)
{
    auto rubricIt = std::find_if(_subrubrics.cbegin(),
                                 _subrubrics.cend(),
                                 [rubric](const std::unique_ptr<Rubric> &rub) {
                                     return rubric == rub.get();
                                 });

    if (rubricIt == _subrubrics.cend())
        return {};

    for (const auto &pair : rubric->_drugs) {
        Drug *drug = pair.first;
        auto degree = pair.second;

        unsigned short maxDegree = 0;
        for (const auto &subRub : _subrubrics)
            if (subRub.get() != rubric)
                maxDegree = std::max<unsigned short>(subRub->drugDegree(drug), maxDegree);

        if (maxDegree == 0) {
            removeDrug(drug);
            drug->removeRubric(this);
        }

        else {
            addDrug(drug, maxDegree);
            drug->addRubric(this, maxDegree);
        }

        drug->addRubric(rubric, degree);
    }
    rubric->_parentRubric = nullptr;

    auto rubricIdx = std::distance(_subrubrics.cbegin(), rubricIt);
    std::unique_ptr<Rubric> rubricPtr = std::move(_subrubrics[rubricIdx]);

    _subrubrics.erase(rubricIt);

    return std::move(rubricPtr);
}

void Rubric::setImportance(unsigned short newImportance)
{
    _importance = newImportance;
}

RubricData::RubricData(const QString &rubricString)
{
    QStringList splitedRubric = rubricString.split(": ");
    if (splitedRubric.size() != 2)
        return;

    _title = splitedRubric[0];

    QStringList unparsedDrugs = splitedRubric[1].split(' ');
    if (unparsedDrugs.isEmpty())
        return;

    if (unparsedDrugs.back().endsWith('.'))
        unparsedDrugs.back().chop(1);

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

        _drugs.insert({drugTitle, drugDegree});
    }
}
