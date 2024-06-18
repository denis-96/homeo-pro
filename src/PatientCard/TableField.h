#ifndef TABLEFIELD_H
#define TABLEFIELD_H

#include <QStyledItemDelegate>
#include <QTableView>
#include "Field.h"

typedef QString RowHeadersFunc(int section);
class TableFieldModel;

class TableField : public Field
{
    Q_OBJECT
public:
    TableField(const QString &label,
               const std::vector<QString> &columnsHeaders,
               RowHeadersFunc *rowHeadersFunc = nullptr,
               int defaultRowCount = 0,
               QWidget *parent = nullptr);

    void read(const QJsonValue &json) override;
    QJsonValue toJson() const override;

private:
    QTableView *view;
    TableFieldModel *model;
    QAction *addRowAction;
    QAction *removeRowAction;
};

class TableFieldModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableFieldModel(const std::vector<QString> &columnsHeaders,
                    RowHeadersFunc *rowHeadersFunc = nullptr,
                    QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override { return _data.size(); }
    int columnCount(const QModelIndex &parent = {}) const override
    {
        return _columnHeaders.size();
    };

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void appendRow(const std::vector<QString> &data = {});
    void removeRow(const QModelIndex &index);

private:
    std::vector<std::vector<QString>> _data;
    std::vector<QString> _columnHeaders;
    RowHeadersFunc *_rowHeadersFunc;
};

class MultiLineEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MultiLineEditDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;
};

#endif // TABLEFIELD_H
