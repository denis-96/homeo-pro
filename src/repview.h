#ifndef REPVIEW_H
#define REPVIEW_H

#include <QTreeView>
#include "repmodel.h"

class RepView : public QTreeView
{
    Q_OBJECT

public:
    explicit RepView(QWidget *parent = nullptr);
    void setRepModel(RepModel *repModel);
};

#endif // REPVIEW_H
