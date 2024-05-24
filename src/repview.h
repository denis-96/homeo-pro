#ifndef REPVIEW_H
#define REPVIEW_H

#include <QHeaderView>
#include <QTreeView>
#include "repmodel.h"

class DrugsView : public QHeaderView
{
public:
    DrugsView(QWidget *parent = nullptr);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
};

class RepView : public QTreeView
{
    Q_OBJECT

public:
    explicit RepView(QWidget *parent = nullptr);
    void setRepModel(RepModel *repModel);
};

#endif // REPVIEW_H
