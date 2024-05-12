#ifndef RUBRIC_H
#define RUBRIC_H

#include <QString>
#include <QStringList>
#include "drug.h"

class RepModel;

class RubricData
{
public:
    RubricData() = default;
    explicit RubricData(const QString &rubricString);
    RubricData(const QString &title, std::unordered_map<QString, unsigned char> drugs);

    bool isEmpty() const;
    const QString &title() const;
    const std::unordered_map<QString, unsigned char> &drugs() const;

private:
    QString _title;
    std::unordered_map<QString, unsigned char> _drugs;
};

inline bool RubricData::isEmpty() const
{
    return _title.isEmpty() && _drugs.empty();
}

inline const QString &RubricData::title() const
{
    return _title;
}

inline const std::unordered_map<QString, unsigned char> &RubricData::drugs() const
{
    return _drugs;
}

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
    void addSubrubric(std::unique_ptr<Rubric> &&rubric);
    std::unique_ptr<Rubric> removeSubrubric(Rubric *rubric);

    Rubric *parentRubric() const;

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

inline int Rubric::subrubricCount() const
{
    return _subrubrics.size();
}

inline Rubric *Rubric::parentRubric() const
{
    return _parentRubric;
}

#endif // RUBRIC_H
