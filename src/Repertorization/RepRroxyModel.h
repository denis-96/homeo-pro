#ifndef REPPROXYMODEL_H
#define REPPROXYMODEL_H

#include <QSortFilterProxyModel>

class RepProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum FilterBehaviour { ChildlessRubrics, ParentRubrics };

    explicit RepProxyModel(QObject *parent = nullptr, FilterBehaviour fBeh = ChildlessRubrics);

    FilterBehaviour filterBehaviour();
    void setFilterBehaviour(FilterBehaviour fBeh);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    FilterBehaviour _filterBehaviour;
};

inline RepProxyModel::FilterBehaviour RepProxyModel::filterBehaviour()
{
    return _filterBehaviour;
}

inline void RepProxyModel::setFilterBehaviour(FilterBehaviour fBeh)
{
    _filterBehaviour = fBeh;

    invalidateRowsFilter();
}

#endif // REPPROXYMODEL_H
