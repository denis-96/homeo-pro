#include "repproxymodel.h"

RepProxyModel::RepProxyModel(QObject *parent, FilterBehaviour fBeh)
    : QSortFilterProxyModel{parent}
    , _filterBehaviour(fBeh)
{}

bool RepProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    auto idx = sourceModel()->index(sourceRow, 0, sourceParent);
    if (_filterBehaviour == ChildlessRubrics)
        return !sourceModel()->hasChildren(idx);
    if (_filterBehaviour == ParentRubrics)
        return sourceModel()->hasChildren(idx);

    return false;
}
