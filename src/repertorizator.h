#ifndef REPERTORIZATOR_H
#define REPERTORIZATOR_H

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QFileDialog>
#include <QIcon>
#include <QItemSelectionModel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>
#include <QToolBar>

#include "repmodel.h"
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
    RepView *repView;

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

    void setupUI();
    void createActions();
    void createMenu();
    void createToolBar();

private slots:
    void newRep();
    void openRep();
    void saveRep();
    void saveRepAs();
    bool closeRep();

    void showContextMenu(QPoint pos);
    void toggleActions();
};

#endif // REPERTORIZATOR_H
