#ifndef MULTILINEEDITDELEGATE_H
#define MULTILINEEDITDELEGATE_H

#include <QStyledItemDelegate>

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

#endif // MULTILINEEDITDELEGATE_H
