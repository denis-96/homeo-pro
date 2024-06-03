#include "repmodel.h"
#include <stack>

RepModel::RepModel(QObject *parent)
    : QAbstractItemModel(parent)
{}

RepModel::~RepModel() = default;

QVariant RepModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    Rubric *rubric = getRubric(index);

    if (!rubric)
        return {};

    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return rubric->title();

        auto drug = _drugs.at(index.column() - 1);
        unsigned char degree = rubric->drugDegree(drug);
        return degree ? degree : QVariant{};
    }
    if (role == Roles::RubricImportance) {
        return rubric->importance();
    }

    return {};
}

QVariant RepModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && section > 0) {
        if (role == Qt::DisplayRole) {
            QString drugTitle;
            drugTitle.reserve(_drugs.at(section - 1).size() * 2);
            for (const auto &ch : _drugs.at(section - 1))
                drugTitle.push_back(ch.toUpper() + '\n');
            drugTitle.removeLast();
            return drugTitle;
        }
        if (role == Qt::UserRole)
            return _drugs.at(section - 1);
    }
    return {};
}

QModelIndex RepModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0)
        return {};

    if (parent.isValid() && parent.column() == 0) {
        if (Rubric *parentRubric = getRubric(parent))
            if (Rubric *rubric = parentRubric->subrubric(row))
                return createIndex(row, column, rubric);
    }

    if (!parent.isValid() && row < rowCount())
        return createIndex(row, column, _rubrics.at(row).get());

    return {};
}

QModelIndex RepModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    Rubric *rubric = getRubric(index);

    if (!rubric)
        return {};

    Rubric *parentRubric = rubric->parentRubric();

    if (!parentRubric)
        return {};

    auto begin = siblings(parentRubric).cbegin(), end = siblings(parentRubric).cend();

    const auto it = std::find_if(begin, end, [parentRubric](const std::unique_ptr<Rubric> &rub) {
        return rub.get() == parentRubric;
    });

    if (it != end)
        return createIndex(std::distance(_rubrics.cbegin(), it), 0, parentRubric);
    return {};
}

int RepModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return _rubrics.size();

    if (parent.column() == 0) {
        if (Rubric *parentRubric = getRubric(parent))
            return parentRubric->subrubricCount();
    }
    return 0;
}
int RepModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _drugs.size() + 1;
}
Qt::ItemFlags RepModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

bool RepModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Roles::RubricImportance) {
        Rubric *rubric = getRubric(index);

        if (!rubric)
            return false;

        bool ok;
        unsigned newImportance = value.toUInt(&ok);
        if (!ok || newImportance > 4)
            return false;

        rubric->setImportance(newImportance);

        update();
        return true;
    }

    return false;
}

QModelIndex RepModel::addRubric(const QString &rubricString)
{
    auto rubric = Rubric::fromString(rubricString);
    if (!rubric->isValid() || findRubric(rubric->title()))
        return {};

    addRubric(std::move(rubric));
    update();
    return index(rowCount() - 1, 0);
}

void RepModel::removeRubric(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int row = index.row();

    if (Rubric *rubric = getRubric(index)) {
        beginRemoveRows(index.parent(), row, row);
        if (rubric->parentRubric())
            rubric->parentRubric()->removeSubrubric(index.row());
        else {
            _rubrics.erase(_rubrics.cbegin() + row);
        }

        endRemoveRows();

        update();
    }
}

void RepModel::removeRubrics(QModelIndexList rubricIndexes)
{
    std::sort(rubricIndexes.begin(),
              rubricIndexes.end(),
              [](const QModelIndex &a, const QModelIndex &b) { return a.row() > b.row(); });

    for (const auto &rubric : rubricIndexes) {
        removeRubric(rubric);
    }
}

QModelIndex RepModel::groupRubrics(QModelIndexList rubricIndexes, const QString &groupTitle)
{
    if (groupTitle.isEmpty() || findRubric(groupTitle))
        return {};

    auto rubricsGroupIdx = addRubric(groupTitle);
    auto rubricsGroup = getRubric(rubricsGroupIdx);

    std::sort(rubricIndexes.begin(),
              rubricIndexes.end(),
              [](const QModelIndex &a, const QModelIndex &b) { return a.row() > b.row(); });

    std::vector<std::unique_ptr<Rubric>> subrubrics;

    for (const auto &rubricIdx : rubricIndexes) {
        int row = rubricIdx.row();
        if (row < rowCount() && _rubrics.at(row).get() == getRubric(rubricIdx)) {
            beginRemoveRows(QModelIndex(), row, row);
            subrubrics.push_back(std::move(_rubrics.at(row)));
            _rubrics.erase(_rubrics.cbegin() + row);
            endRemoveRows();
        }
    }
    beginInsertRows(index(rowCount() - 1, 0),
                    rubricsGroup->subrubricCount(),
                    rubricsGroup->subrubricCount() + subrubrics.size() - 1);

    for (auto &subrubric : subrubrics) {
        rubricsGroup->addSubrubric(std::move(subrubric));
    }
    endInsertRows();

    update();

    return rubricsGroupIdx;
}

void RepModel::ungroupRubrics(const QModelIndex &rubricsGroup)
{
    Rubric *parentRubric = getRubric(rubricsGroup);

    if (!parentRubric)
        return;

    std::vector<std::unique_ptr<Rubric>> subrubrics;

    beginRemoveRows(rubricsGroup, 0, parentRubric->subrubricCount() - 1);
    for (int rubNum = parentRubric->subrubricCount() - 1; rubNum >= 0; --rubNum) {
        subrubrics.push_back(parentRubric->removeSubrubric(rubNum));
    }
    endRemoveRows();

    beginInsertRows(QModelIndex(), rubricsGroup.row(), rubricsGroup.row() + subrubrics.size() - 1);
    auto insertPos = _rubrics.cbegin() + rubricsGroup.row();
    for (auto &subrubric : subrubrics) {
        insertPos = _rubrics.insert(insertPos, std::move(subrubric));
        ++insertPos;
    }
    endInsertRows();

    removeRubric(index(insertPos - _rubrics.cbegin(), 0));
}

void RepModel::fromString(const QString &repStr)
{
    clear();

    QList<QStringView> lines = QStringView{repStr}.split(u'\n');

    struct ParentIndentation
    {
        Rubric *parent;
        QModelIndex parentIdx;
        int indentation;
    };

    std::stack<ParentIndentation> state;
    state.push({nullptr, QModelIndex(), 0});

    for (const auto &line : lines) {
        int position = 0;
        for (; position < line.length() && line.at(position).isSpace(); ++position) {
        }

        const QStringView lineData = line.sliced(position).trimmed();
        if (!lineData.isEmpty()) {
            // Read the data.
            auto rubric = Rubric::fromString(lineData.toString());
            if (!rubric->isValid() || findRubric(rubric->title()))
                continue;

            if (position > state.top().indentation) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.
                Rubric *lastParent = state.top().parent;
                const auto &children = lastParent ? lastParent->_subrubrics : _rubrics;
                if (!children.empty()) {
                    Rubric *child = children.back().get();
                    state.push({child, createIndex(children.size() - 1, 0, child), position});
                }

            } else {
                while (position < state.top().indentation && !state.empty())
                    state.pop();
            }
            addRubric(std::move(rubric), state.top().parentIdx);
        }
    }

    update();
}

QString RepModel::toString()
{
    QString result;

    struct StateValue
    {
        Rubric *rubric;
        int indentation;
    };

    std::stack<StateValue> state;

    for (auto it = _rubrics.crbegin(); it < _rubrics.crend(); ++it)
        state.push({it->get(), 0});

    while (!state.empty()) {
        auto current = state.top();
        state.pop();

        for (int i = 0; i < current.indentation; ++i)
            result.push_back(' ');

        result.push_back(current.rubric->toString());
        result.push_back('\n');
        for (auto it = current.rubric->_subrubrics.crbegin();
             it < current.rubric->_subrubrics.crend();
             ++it)
            state.push({it->get(), current.indentation + 1});
    }

    return result;
}

void RepModel::clear()
{
    if (rowCount() > 0) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        _rubrics.clear();
        endRemoveRows();
    }

    if (columnCount() > 1) {
        beginRemoveColumns(QModelIndex(), 1, columnCount() - 1);
        _drugs.clear();
        endRemoveColumns();
    }
}

void RepModel::update()
{
    struct DrugInfo
    {
        int rubrics = 0;
        int points = 0;
    };

    std::unordered_map<QString, DrugInfo> drugsInfo;

    for (const auto &rubric : _rubrics) {
        for (const auto &drug : rubric->_drugs) {
            int coeff = rubric->importance();
            ++drugsInfo[drug.first].rubrics;
            drugsInfo[drug.first].points += coeff * (drug.second + 4);
        }
    }

    std::sort(_drugs.begin(),
              _drugs.end(),
              [&drugsInfo](const QString &d1, const QString &d2) -> bool {
                  DrugInfo d1Info = drugsInfo[d1];
                  DrugInfo d2Info = drugsInfo[d2];
                  if (d1Info.points == d2Info.points) {
                      return d1 < d2;
                  }
                  return d1Info.points > d2Info.points;
              });

    auto deletionBegIt = std::find_if(_drugs.crbegin(),
                                      _drugs.crend(),
                                      [&drugsInfo](const QString &d) {
                                          return drugsInfo[d].rubrics > 0;
                                      });

    auto deletionBegIdx = _drugs.crend() - deletionBegIt;

    if (deletionBegIt != _drugs.crbegin()) {
        beginRemoveColumns(QModelIndex(), deletionBegIdx + 1, columnCount() - 1);
        _drugs.erase(deletionBegIt.base(), _drugs.cend());
        endRemoveColumns();
    }

    emit headerDataChanged(Qt::Horizontal, 1, columnCount() - 1);
    emit dataChanged(index(0, 1), index(rowCount() - 1, columnCount() - 1), {Qt::DisplayRole});
}

void RepModel::addRubric(std::unique_ptr<Rubric> &&rubric, const QModelIndex &parent)
{
    auto newDrugs = rubric->_drugs;
    // Existing drug
    for (const auto &drug : _drugs) {
        if (rubric->_drugs.find(drug) != rubric->_drugs.end()) {
            newDrugs.erase(drug);
        }
    }
    // New drugs
    if (newDrugs.size()) {
        beginInsertColumns(QModelIndex(), columnCount(), columnCount() + newDrugs.size() - 1);
        for (const auto &drug : newDrugs) {
            _drugs.push_back(drug.first);
        }
        endInsertColumns();
    }

    // Insert rubric
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
    if (auto parentPtr = getRubric(parent)) {
        parentPtr->addSubrubric(std::move(rubric));

    } else
        _rubrics.push_back(std::move(rubric));
    endInsertRows();
}

Rubric *RepModel::findRubric(const QString &title, Rubric *parent) const
{
    auto it = _rubrics.cbegin(), end = _rubrics.cend();
    if (parent)
        it = parent->_subrubrics.cbegin(), end = parent->_subrubrics.cend();

    for (; it != end; ++it) {
        if ((*it)->title() == title)
            return it->get();
        if (Rubric *found = findRubric(title, it->get()))
            return found;
    }

    return nullptr;
}
