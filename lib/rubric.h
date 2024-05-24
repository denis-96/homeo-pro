#ifndef RUBRIC_H
#define RUBRIC_H

#include <QString>
#include <QStringList>

class RepModel;

class Rubric
{
    friend RepModel;

public:
    using Drugs = std::unordered_map<QString, unsigned char>;

    explicit Rubric(const QString &title = "",
                    const Drugs &drugs = {},
                    Rubric *parentRubric = nullptr);

    ~Rubric();

    bool isValid();

    const QString &title() const;
    void setTitle(const QString &newTitle);

    unsigned short importance() const;
    void setImportance(unsigned short newImportance);

    int drugCount() const;
    unsigned char drugDegree(const QString &drug) const;
    bool addDrug(const QString &drug, unsigned char degree);
    bool removeDrug(const QString &drug);

    Rubric *subrubric(int number);
    int subrubricCount() const;
    void addSubrubric(std::unique_ptr<Rubric> &&rubric);
    std::unique_ptr<Rubric> removeSubrubric(int number);

    Rubric *parentRubric() const;

    static std::unique_ptr<Rubric> fromString(const QString &str);
    QString toString();

private:
    QString _title;
    unsigned short _importance = 1;
    Drugs _drugs;

    Rubric *_parentRubric;
    std::vector<std::unique_ptr<Rubric>> _subrubrics;
};

inline bool Rubric::isValid()
{
    return _title.size();
}

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
