#if 0
#include "drug.h"
#include <QDebug>
#include "rubric.h"

Drug::Drug(const QString &title, const Rubrics &rubrics)
    : _title(title)
    , _rubrics(rubrics)
{
    qDebug() << "Drug with title" << _title << "created.";
}

Drug::~Drug()
{
    for (const auto &rubric : _rubrics) {
        rubric.first->removeDrug(this);
    }
    qDebug() << "Drug with title" << _title << "deleted.";
}

DrugInfo Drug::info() const
{
    int rubricCount = 0;
    int degreeSum = 0;

    for (const auto &rubric : _rubrics) {
        rubricCount += rubric.first->importance();
        degreeSum += rubric.second * rubric.first->importance();
    }

    return {rubricCount, degreeSum};
}

void Drug::addRubric(Rubric *rubric, unsigned char degree)
{
    if (degree <= 4)
        _rubrics[rubric] = degree;
}

void Drug::removeRubric(Rubric *rubric)
{
    _rubrics.erase(rubric);
}
#endif
