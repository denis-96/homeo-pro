#include "rubric.h"

Rubric::Rubric(const QString &title, const Drugs &drugs, Rubric *parentRubric)
    : _title(title)
    , _drugs(drugs)
    , _parentRubric(parentRubric)
{

}

Rubric::~Rubric()
{
    for (const auto &drug : _drugs) {
        drug.first->removeRubric(this);
    }
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
    if (degree > 4)
        return;
    _drugs.insert({drug, degree});
}

void Rubric::removeDrug(Drug *drug)
{
    _drugs.erase(drug);
}

void Rubric::setImportance(unsigned short newImportance)
{
    _importance = newImportance;
}
