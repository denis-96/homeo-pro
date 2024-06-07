#ifndef REPERTORIZATOR_H
#define REPERTORIZATOR_H

#include <QListView>
#include <QMainWindow>
#include "repmodel.h"
#include "repproxymodel.h"
#include "repview.h"

class Repertorizator : public QMainWindow
{
    Q_OBJECT

public:
    explicit Repertorizator(QWidget *parent = nullptr);
    ~Repertorizator();

    bool readRep(const QString &fileName);
    bool writeRep(const QString &fileName);

    void updateTitle(int state);
    void closeEvent(QCloseEvent *event);

private:
    QString currentRep;

    RepModel *repModel;
    RepProxyModel *repProxyModel;
    RepView *repView;
    QListView *rubricsView;

    QMenu *repMenu;
    QMenu *editMenu;
    QMenu *contextMenu;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;

    QAction *addRubricAction;
    QAction *removeRubricAction;
    QAction *groupRubricsAction;
    QAction *ungroupRubricsAction;

    QActionGroup *changeImportanceActions;

    QAction *removeSubrubAction;
    QAction *addSubrubAction;

    QToolBar *toolBar;

    QDialog *groupCreationDialog;

    void setupUI();
    void createActions();
    void createMenu();
    void createToolBar();
    void createDialogs();

    static QString generateTitleForGroup(const QModelIndexList &rubrics);

private slots:
    void newRep();
    void openRep();
    void saveRep();
    void saveRepAs();
    bool closeRep();

    void addRubric();
    void removeRubric();
    void groupRubrics();
    void ungroupRubrics();

    void showContextMenu(QPoint pos);
    void toggleActions();

public slots:
    void openWith();
};

#endif // REPERTORIZATOR_H
