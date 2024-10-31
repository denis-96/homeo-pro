#include "MultiLineEditDelegate.h"

#include <QPlainTextEdit>

MultiLineEditDelegate::MultiLineEditDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}
QWidget *MultiLineEditDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    QPlainTextEdit *editor = new QPlainTextEdit(parent);
    editor->setAutoFillBackground(true);
    editor->setBackgroundRole(QPalette::Window);
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
