#ifndef RUBRIC_H
#define RUBRIC_H

#include <QString>
#include "drug.h"

class RepModel;
class Rubric
{
    friend RepModel;

public:
    using Drugs = std::unordered_map<Drug *, unsigned char>;

    explicit Rubric(const QString &title, const Drugs &drugs = {}, Rubric *parentRubric = nullptr);
    ~Rubric();

    const QString &title() const;
    unsigned short importance() const;
    void setImportance(unsigned short newImportance);
    int drugCount() const;

    unsigned char drugDegree(Drug *drug) const;
    void addDrug(Drug *drug, unsigned char degree);
    void removeDrug(Drug *drug);

    Rubric *subrubric(int number);
    int subrubricCount() const;
    void addSubrubric(Rubric *rubric);
    Rubric *parentRubric();

private:
    QString _title;
    unsigned short _importance = 1;
    Drugs _drugs;

    Rubric *_parentRubric;
    std::vector<std::unique_ptr<Rubric>> _subrubrics;
};

inline const QString &Rubric::title() const
{
    return _title;
}

inline unsigned short Rubric::importance() const
{
    return _importance;
}

inline int Rubric::drugCount() const
{
    return _drugs.size();
}

inline Rubric *Rubric::subrubric(int number)
{
    if (number < subrubricCount())
        return _subrubrics[number].get();
    return nullptr;
}

inline int Rubric::subrubricCount() const
{
    return _subrubrics.size();
}

inline Rubric *Rubric::parentRubric()
{
    return _parentRubric;
}

#endif // RUBRIC_H
