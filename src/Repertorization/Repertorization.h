#ifndef REPERTORIZATION_H
#define REPERTORIZATION_H

#include <QFile>
#include <QListView>
#include <QToolBar>
#include <QWidget>
#include "RepModel.h"
#include "RepRroxyModel.h"
#include "RepView.h"
#include <HomeoEntity.h>

class Repertorization : public HomeoEntity
{
    Q_OBJECT
public:
    explicit Repertorization(QWidget *parent = nullptr);

private:
    bool read(QFile &file) override;
    bool write(QFile &file) override;

    RepModel *repModel;
    RepProxyModel *repProxyModel;
    RepView *repView;
    QListView *rubricsView;

    QMenu *contextMenu;
    QToolBar *toolBar;
    QDialog *groupCreationDialog;

    QAction *addRubricAction;
    QAction *removeRubricAction;
    QAction *groupRubricsAction;
    QAction *ungroupRubricsAction;

    QActionGroup *changeImportanceActions;

    QAction *removeSubrubAction;
    QAction *addSubrubAction;

    void setupUI();
    void createActions();
    void createContextMenu();
    void createToolBar();
    void createDialogs();

    static QString generateTitleForGroup(const QModelIndexList &rubrics);

private slots:
    void addRubric();
    void removeRubric();
    void groupRubrics();
    void ungroupRubrics();
    void changeRubricImportance();

    void showContextMenu(QPoint pos);
    void updateActions();
};

#endif // REPERTORIZATION_H
