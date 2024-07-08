#include "TableField.h"
#include <QHeaderView>
#include <QJsonArray>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

TableField::TableField(const QString &label,
                       const std::vector<QString> &columnsHeaders,
                       const RowHeadersFunc &rowHeadersFunc,
                       int defaultRowCount,
                       QWidget *parent)
    : Field(parent)
{
    model = new TableFieldModel(columnsHeaders, rowHeadersFunc, this);
    connect(model, &QAbstractItemModel::dataChanged, this, &Field::changed);
    connect(model, &QAbstractItemModel::rowsInserted, this, &Field::changed);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &Field::changed);

    for (int i = 0; i < defaultRowCount; ++i) {
        model->appendRow();
    }

    view = new QTableView(this);
    view->setModel(model);
    view->setColumnWidth(0, 300);
    view->setMinimumHeight(500);
    view->setWordWrap(true);
    view->horizontalHeader()->setStretchLastSection(true);
    auto f = view->horizontalHeader()->font();
    f.setPointSize(14);
    f.setBold(true);
    view->horizontalHeader()->setFont(f);
    view->verticalHeader()->setDefaultSectionSize(60);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setItemDelegate(new MultiLineEditDelegate(this));

    addRowAction = new QAction(QIcon(":/resources/icons/add.svg"), "Добавить запись", this);
    connect(addRowAction, &QAction::triggered, this, [this] { model->appendRow(); });
    addAction(addRowAction);

    removeRowAction = new QAction(QIcon(":/resources/icons/remove.svg"), "Удалить запись", this);
    removeRowAction->setDisabled(true);
    connect(removeRowAction, &QAction::triggered, this, [this] {
        model->removeRow(view->selectionModel()->selectedRows().back());
    });
    addAction(removeRowAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this] {
        removeRowAction->setEnabled(view->selectionModel()->selectedRows().size());
    });

    setLayout(new QVBoxLayout(this));
    if (label.size()) {
        auto lbl = new QLabel(label, this);
        lbl->setWordWrap(true);
        layout()->addWidget(lbl);
    } else
        layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(view);
}

void TableField::addRow(const std::vector<QString> &data)
{
    model->appendRow(data);
}

void TableField::read(const QJsonValue &json)
{
    if (!json.isArray())
        return;

    QJsonArray tableJson = json.toArray();

    for (const QJsonValue &row : tableJson) {
        if (!row.isArray())
            continue;
        const QJsonArray cols = row.toArray();
        if (cols.size() != model->columnCount())
            continue;
        std::vector<QString> rowData(cols.size());
        for (int i = 0; i < model->columnCount(); ++i) {
            rowData.at(i) = cols.at(i).toString();
        }
        model->appendRow(rowData);
    }

    view->resizeRowsToContents();
}

QJsonValue TableField::toJson() const
{
    QJsonArray json;
    for (int i = 0; i < model->rowCount(); ++i) {
        QJsonArray row;
        for (int j = 0; j < model->columnCount(); ++j) {
            row.push_back(model->index(i, j).data().toString());
        }
        json.push_back(row);
    }
    return json;
}

TableFieldModel::TableFieldModel(const std::vector<QString> &columnsHeaders,
                                 const RowHeadersFunc &rowHeadersFunc,
                                 QObject *parent)
    : QAbstractTableModel{parent}
    , _rowHeadersFunc(rowHeadersFunc)
    , _columnHeaders(columnsHeaders)
{}

QVariant TableFieldModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return _data.at(index.row()).at(index.column());
    }

    return {};
}

bool TableFieldModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {
        _data.at(index.row()).at(index.column()) = value.toString();
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
        return true;
    }

    return false;
}

QVariant TableFieldModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal)
            return _columnHeaders.at(section);
        if (orientation == Qt::Vertical && _rowHeadersFunc) {
            return _rowHeadersFunc(section);
        }
    }
    return {};
}

Qt::ItemFlags TableFieldModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

void TableFieldModel::appendRow(const std::vector<QString> &data)
{
    if (data.empty()) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        _data.push_back(std::vector<QString>(columnCount()));
        endInsertRows();
    } else if (data.size() == columnCount()) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        _data.push_back(data);
        endInsertRows();
    }
}

void TableFieldModel::removeRow(const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    _data.erase(_data.begin() + index.row());
    endRemoveRows();
}

MultiLineEditDelegate::MultiLineEditDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}
QWidget *MultiLineEditDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    QPlainTextEdit *editor = new QPlainTextEdit(parent);
    editor->setContentsMargins(0, 0, 0, 0);
    return editor;
}

void MultiLineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QPlainTextEdit *mEditor = qobject_cast<QPlainTextEdit *>(editor);
    mEditor->setPlainText(index.data().toString());
}

void MultiLineEditDelegate::setModelData(QWidget *editor,
                                         QAbstractItemModel *model,
                                         const QModelIndex &index) const
{
    QPlainTextEdit *mEditor = qobject_cast<QPlainTextEdit *>(editor);
    model->setData(index, mEditor->toPlainText());
}
