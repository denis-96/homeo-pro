#include "repmodel.h"
#include "rubric.h"
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
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return rubric->title();

        Drug *drug = _drugs[index.column() - 1].get();
        unsigned char degree = rubric->drugDegree(drug);
        return degree ? degree : QVariant{};
    }
    if (role == Roles::RubricImportance && index.column() == 0) {
        return rubric->importance();
    }

    return {};
}

QVariant RepModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)

    {
        return section > 0 ? _drugs.at(section - 1)->title() : "Рубрики \\ Препараты";
    }
    return {};
}

QModelIndex RepModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return row >= 0 && row < rowCount() ? createIndex(row, column, _rubrics.at(row).get())
                                            : QModelIndex{};

    if (parent.column() == 0) {
        if (Rubric *parentRubric = getRubric(parent))
            if (Rubric *rubric = parentRubric->subrubric(row))
                return createIndex(row, column, rubric);
    }
    return {};
}

QModelIndex RepModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    Rubric *rubric = getRubric(index);
    Rubric *parentRubric = rubric ? rubric->parentRubric() : nullptr;

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
    if (role != Roles::RubricImportance || index.column() != 0)
        return false;

    Rubric *rubric = getRubric(index);

    if (!rubric)
        return false;

    bool ok;
    unsigned newImportance = value.toUInt(&ok);
    if (!ok || newImportance > 4)
        return false;

    rubric->setImportance(newImportance);

    return true;
}

void RepModel::addRubric(const QString &rubricString)
{
    if (Rubric *rubric = rubricFromString(rubricString)) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        _rubrics.push_back(std::unique_ptr<Rubric>(rubric));
        endInsertRows();
        update();
    }
}

void RepModel::removeRubric(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int row = index.row();

    // TODO: remove subrubric
    if (Rubric *rubric = getRubric(index)) {
        beginRemoveRows(index.parent(), row, row);
        _rubrics.erase(_rubrics.cbegin() + row);
        endRemoveRows();

        update();
    }
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
            // Read the data from the rest of the line.
            Rubric *rubric = rubricFromString(lineData.toString());
            if (!rubric)
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

            // Append a new item to the current parent's list of children.
            Rubric *parent = state.top().parent;
            QModelIndex parentIdx = state.top().parentIdx;
            beginInsertRows(parentIdx, rowCount(parentIdx), rowCount(parentIdx));

            if (parent) {
                rubric->_parentRubric = parent;
                parent->_subrubrics.push_back(std::unique_ptr<Rubric>(rubric));
            } else
                _rubrics.push_back(std::unique_ptr<Rubric>(rubric));
            endInsertRows();
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
            result += ' ';

        result += rubricToString(current.rubric);
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
    std::unordered_map<Drug *, DrugInfo> drugsInfo;
    int drugsToRemove{0};

    for (const auto &drug : _drugs) {
        drugsInfo.insert({drug.get(), drug->info()});
        if (drug->rubricCount() == 0)
            ++drugsToRemove, drugsInfo[drug.get()].rubricCount = -99999;
    }

    std::sort(_drugs.begin(),
              _drugs.end(),
              [&drugsInfo](const std::unique_ptr<Drug> &d1,
                           const std::unique_ptr<Drug> &d2) -> bool {
                  DrugInfo d1Info = drugsInfo[d1.get()];
                  DrugInfo d2Info = drugsInfo[d2.get()];
                  if (d1Info.rubricCount == d2Info.rubricCount) {
                      if (d1Info.degreeSum == d2Info.degreeSum)
                          return d1->title() < d2->title();
                      return d1Info.degreeSum > d2Info.degreeSum;
                  }
                  return d1Info.rubricCount > d2Info.rubricCount;
              });

    if (drugsToRemove) {
        beginRemoveColumns(QModelIndex(), columnCount() - drugsToRemove, columnCount() - 1);
        _drugs.erase(_drugs.cend() - drugsToRemove, _drugs.cend());
        endRemoveColumns();
    }

    emit headerDataChanged(Qt::Horizontal, 1, columnCount() - 1);
    emit dataChanged(index(0, 1), index(rowCount() - 1, columnCount() - 1), {Qt::DisplayRole});
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

Rubric *RepModel::rubricFromString(const QString &rubricString)
{
    QString rubricTitle;
    std::unordered_map<QString, unsigned char> rubricDrugs;

    QStringList splitedRubric = rubricString.split(": ");
    if (splitedRubric.size() != 2)
        return nullptr;

    rubricTitle = splitedRubric[0];

    if (findRubric(rubricTitle))
        return nullptr;

    QStringList unparsedDrugs = splitedRubric[1].split(' ');
    if (unparsedDrugs.isEmpty())
        return nullptr;

    if (unparsedDrugs.back().endsWith('.'))
        unparsedDrugs.back().chop(1);

    for (auto &drug : unparsedDrugs) {
        QStringList splitedDrug = drug.split("(");
        if (splitedDrug.size() > 2)
            return nullptr;

        QString drugTitle = splitedDrug[0].toLower();
        quint8 drugDegree = 1;
        if (splitedDrug.size() == 2) {
            bool ok;
            drugDegree = splitedDrug[1].chopped(1).toUShort(&ok);
            if (!ok)
                return nullptr;
        }

        rubricDrugs.insert({drugTitle, drugDegree});
    }

    // Create new rubric
    Rubric *rubric = new Rubric(rubricTitle);

    // Existing drug
    for (const auto &drug : _drugs) {
        QString drugTitle = drug->title();
        if (rubricDrugs.find(drugTitle) != rubricDrugs.end()) {
            unsigned char degree = rubricDrugs.at(drugTitle);
            rubric->addDrug(drug.get(), degree);
            drug->addRubric(rubric, degree);
            rubricDrugs.erase(drugTitle);
        }
    }

    // New drugs
    beginInsertColumns(QModelIndex(), columnCount(), columnCount() + rubricDrugs.size() - 1);
    for (const auto &variant : rubricDrugs) {
        std::unique_ptr<Drug> drug(new Drug(variant.first));

        rubric->addDrug(drug.get(), variant.second);
        drug->addRubric(rubric, variant.second);

        _drugs.push_back(std::move(drug));
    }
    endInsertColumns();

    return rubric;
}

QString RepModel::rubricToString(Rubric *rubric) const
{
    QString rubStr(rubric->title());
    rubStr += ":";

    for (const auto &drug : rubric->_drugs)
        rubStr += ' ' + drug.first->title() + '(' + QString::number(drug.second) + ')';

    rubStr += ".\n";

    return rubStr;
}
