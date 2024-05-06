#include "repmodel.h"
#include <QFile>
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
    Rubric *grandParentRubric = parentRubric ? parentRubric->parentRubric() : nullptr;

    if (!parentRubric)
        return {};

    auto begin = _rubrics.cbegin(), end = _rubrics.cend();
    if (grandParentRubric)
        begin = grandParentRubric->_subrubrics.cbegin(),
        end = grandParentRubric->_subrubrics.cend();

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

void RepModel::addRubric(const QString &rubricString, const QModelIndex &parent)
{
    if (Rubric *rubric = rubricFromString(rubricString)) {
        // Append rubric
        beginInsertRows(parent, rowCount(parent), rowCount(parent));
        parent.isValid() ? getRubric(parent)->_subrubrics.push_back(std::unique_ptr<Rubric>(rubric))
                         : _rubrics.push_back(std::unique_ptr<Rubric>(rubric));

        endInsertRows();
        update();
    }
}

void RepModel::removeRubric(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int row = index.row();

    beginRemoveRows(QModelIndex(), row, row);
    _rubrics.erase(_rubrics.cbegin() + row);
    endRemoveRows();

    update();
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
                if (lastParent) {
                    if (lastParent->subrubricCount() > 0)
                        state.push(
                            {lastParent->subrubric(lastParent->subrubricCount() - 1),
                             createIndex(lastParent->subrubricCount() - 1,
                                         0,
                                         lastParent->subrubric(lastParent->subrubricCount() - 1)),
                             position});
                } else {
                    if (_rubrics.size() > 0)
                        state.push({_rubrics.back().get(),
                                    createIndex(_rubrics.size() - 1, 0, _rubrics.back().get()),
                                    position});
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

/*
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        return false;
    }

    clear();

    QTextStream dataStream(&file);

    int drugCount = 0;
    dataStream >> drugCount;
    if (!drugCount) {
        file.close();
        return false;
    }

    beginInsertColumns(QModelIndex(), 1, drugCount);
    for (int i = 0; i < drugCount; ++i) {
        quint32 id;
        Drug drug{QString(), 0, 0, 0};
        dataStream >> id;
        dataStream.readLine();
        dataStream.readLineInto(&drug.title);

        drugs.insert(id, drug);
        sortedDrugs.append(id);

        lastDrugId = std::max(lastDrugId, id);
    }
    endInsertColumns();

    int rubricCount = 0;
    dataStream >> rubricCount;
    if (!rubricCount) {
        file.close();
        return false;
    }
    beginInsertRows(QModelIndex(), 0, rubricCount - 1);
    for (int i = 0; i < rubricCount; ++i) {
        Rubric rubric;
        int checkState;
        dataStream.readLine();
        dataStream.readLineInto(&rubric.title);
        dataStream >> checkState;
        rubric.checkState = checkState ? Qt::Checked : Qt::Unchecked;

        int drugVariantsCount = 0;
        dataStream >> drugVariantsCount;

        if (!drugVariantsCount) {
            file.close();
            return false;
        }

        for (int j = 0; j < drugVariantsCount; ++j) {
            quint32 drugId;
            short degree;
            dataStream >> drugId >> degree;
            rubric.drugVariants.insert(drugId, degree);

            Drug &drug = drugs[drugId];
            drug.variantsCount++;

            if (rubric.checkState == Qt::Unchecked)
                drug.disabledVariantsCnt++;
            else
                drug.variantsSum += degree;
        }

        rubrics.append(rubric);
    }
    endInsertRows();

    file.close();

    sort();
    emit headerDataChanged(Qt::Horizontal, 1, columnCount() - 1);
    emit dataChanged(index(0, 1), index(rowCount() - 1, columnCount() - 1), {Qt::DisplayRole});

    */

/*
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    QTextStream dataStream(&file);

    int drugCount = drugs.count();

    if (!drugCount) {
        file.close();
        return false;
    }

    dataStream << drugCount << '\n';
    for (auto drug = drugs.cbegin(), end = drugs.cend(); drug != end; ++drug) {
        dataStream << drug.key() << '\n' << drug.value().title << '\n';
    }

    int rubricCount = rubrics.count();

    if (!rubricCount) {
        file.close();
        return false;
    }

    dataStream << rubricCount << '\n';
    for (const Rubric &rubric : rubrics) {
        dataStream << rubric.title << '\n';

        dataStream << rubric.checkState << ' ';

        int drugVariantsCount = rubric.drugVariants.count();
        if (!drugVariantsCount) {
            file.close();
            return false;
        }

        dataStream << drugVariantsCount;
        for (auto variant = rubric.drugVariants.cbegin(), end = rubric.drugVariants.cend();
             variant != end;
             ++variant) {
            dataStream << ' ' << variant.key() << ' ' << variant.value();
        }
        dataStream << '\n';
    }
    */

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

QString RepModel::rubricToString(Rubric *rubric)
{
    QString rubStr(rubric->title());
    rubStr += ":";

    for (const auto &drug : rubric->_drugs)
        rubStr += ' ' + drug.first->title() + '(' + QString::number(drug.second) + ')';

    rubStr += ".\n";

    return rubStr;
}
