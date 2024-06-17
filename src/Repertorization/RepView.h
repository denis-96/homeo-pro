#ifndef REPVIEW_H
#define REPVIEW_H

#include <QTreeView>

class RepView : public QTreeView
{
    Q_OBJECT

public:
    explicit RepView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) override;
};

#endif // REPVIEW_H
