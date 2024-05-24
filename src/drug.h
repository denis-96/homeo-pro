#ifndef DRUG_H
#define DRUG_H

#if 0
#include <QString>

class Rubric;

struct DrugInfo
{
    int rubricCount = 0;
    int degreeSum = 0;
};

class Drug
{
public:
    using Rubrics = std::unordered_map<Rubric *, unsigned char>;

public:
    explicit Drug(const QString &title, const Rubrics &rubrics = {});
    ~Drug();

    const QString &title() const;
    DrugInfo info() const;

    int rubricCount() const;
    void addRubric(Rubric *rubric, unsigned char degree);
    void removeRubric(Rubric *rubric);

private:
    QString _title;
    Rubrics _rubrics;
};

inline const QString &Drug::title() const
{
    return _title;
}

inline int Drug::rubricCount() const
{
    return _rubrics.size();
}
#endif
#endif // DRUG_H
