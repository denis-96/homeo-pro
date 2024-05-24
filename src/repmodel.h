#ifndef REPMODEL_H
#define REPMODEL_H

#include <QAbstractItemModel>
#include "rubric.h"

class RepModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    Q_DISABLE_COPY_MOVE(RepModel)
    enum Roles { RubricImportance = Qt::UserRole + 1 };

    RepModel(QObject *parent = nullptr);
    ~RepModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    bool isParent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void addRubric(const QString &rubricString, const QModelIndex &parent = {});
    void removeRubric(const QModelIndex &index);
    void removeRubrics(QModelIndexList rubricIndexes);

    void groupRubrics(QModelIndexList rubricIndexes);
    void ungroupRubrics(const QModelIndex &rubricsGroup);

    void fromString(const QString &repStr);
    QString toString();

    void clear();

private:
    void update();

    void addRubric(std::unique_ptr<Rubric> &&rubric, const QModelIndex &parent = {});
    Rubric *getRubric(const QModelIndex &index) const;
    Rubric *findRubric(const QString &title, Rubric *parent = nullptr) const;
    const std::vector<std::unique_ptr<Rubric>> &siblings(Rubric *rubric) const;

    std::vector<std::unique_ptr<Rubric>> _rubrics;
    std::vector<QString> _drugs;
};

inline bool RepModel::isParent(const QModelIndex &index) const
{
    if (Rubric *rubric = getRubric(index))
        return rubric->subrubricCount();
    return false;
}

inline Rubric *RepModel::getRubric(const QModelIndex &index) const
{
    return static_cast<Rubric *>(index.internalPointer());
}
inline const std::vector<std::unique_ptr<Rubric>> &RepModel::siblings(Rubric *rubric) const
{
    return rubric->parentRubric() ? rubric->parentRubric()->_subrubrics : _rubrics;
}

#endif // REPMODEL_H
