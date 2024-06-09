#ifndef HOMEOPRO_H
#define HOMEOPRO_H

#include <QMainWindow>
#include <HomeoEntity.h>

class HomePro : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomePro(QWidget *parent = nullptr);
    ~HomePro();

    void closeEvent(QCloseEvent *event);

private:
    QTabWidget *tabWidget;
    QDialog *newFileDialog;

    QAction *newFileAction;
    QAction *openAction;

    QAction *saveAction;
    QAction *saveAllAction;

    QAction *closeAction;
    QAction *closeAllAction;

    QAction *exitAction;

    void createActions();
    void createMenu();
    void createDialogs();

    void addHomeoEntity(HomeoEntity *homeoEntity, const QString &title);

    bool readFile(const QString &fileName);

    void updateTabTitle(int tabIdx, bool isPageSaved, const QString &title = "");

    static QString getTitleFromFileName(const QString &fileName);

private slots:
    void newFile();
    void open();
    void save(int fileIdx = -1);
    void saveAll();
    void close(int fileIdx = -1);
    void closeAll();
    void exit();

    void updateActions();

public slots:
    void openWith();
};

#endif // REPERTORIZATOR_H
