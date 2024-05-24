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
    qDebug() << "Rubric with title" << _title << "deleted.";
}

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
void Rubric::addDrug(const QString &drug, unsigned char degree)
{
    if (degree <= 4)
        _drugs[drug] = degree;
}

void Rubric::removeDrug(const QString &drug)
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

    return std::move(rubricPtr);
}

std::unique_ptr<Rubric> Rubric::fromString(const QString &str)
{
    QStringList splitedRubric = str.split(": ");
    if (splitedRubric.size() != 2)
        return std::make_unique<Rubric>();

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
    return std::move(rubric);
}

QString Rubric::toString()
{
    QString rubStr(title());
    rubStr += ":";

    for (const auto &drug : _drugs)
        rubStr += ' ' + drug.first + '(' + QString::number(drug.second) + ')';

    rubStr += ".\n";

    return rubStr;
}
