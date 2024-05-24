#include "repertorizator.h"

Repertorizator::Repertorizator(QWidget *parent)
    : QMainWindow(parent)
{
    updateTitle(1);
    resize(900, 500);

    repModel = new RepModel(this);
    repView = new RepView();

    repView->setRepModel(repModel);

    setupUI();
}
Repertorizator::~Repertorizator() {}

void Repertorizator::setupUI()
{
    createActions();
    createMenu();
    createToolBar();

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,
            &Repertorizator::customContextMenuRequested,
            this,
            &Repertorizator::showContextMenu);

    connect(repView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &Repertorizator::toggleActions);
    connect(repModel, &RepModel::dataChanged, this, [this] { updateTitle(2); });
    this->setCentralWidget(repView);
    this->setWindowIcon(QIcon(":/icons/main-icon"));
}

void Repertorizator::createActions()
{
    newAction = new QAction("Новая", this);
    newAction->setIcon(QIcon(":/icons/new-file"));
    connect(newAction, &QAction::triggered, this, &Repertorizator::newRep);

    openAction = new QAction("Открыть", this);
    openAction->setIcon(QIcon(":/icons/open-file"));
    connect(openAction, &QAction::triggered, this, &Repertorizator::openRep);

    saveAction = new QAction("Сохранить", this);
    saveAction->setIcon(QIcon(":/icons/save-file"));
    connect(saveAction, &QAction::triggered, this, &Repertorizator::saveRep);

    saveAsAction = new QAction("Сохранить как", this);
    saveAsAction->setIcon(QIcon(":/icons/save-file"));
    connect(saveAsAction, &QAction::triggered, this, &Repertorizator::saveRepAs);

    exitAction = new QAction("Выход", this);
    exitAction->setIcon(QIcon(":/icons/exit"));
    connect(exitAction, &QAction::triggered, this, &Repertorizator::close);

    addRubricAction = new QAction("Добавить", this);
    addRubricAction->setIcon(QIcon(":/icons/add"));
    connect(addRubricAction, &QAction::triggered, this, [this] {
        QString rubricStr = QApplication::clipboard()->text();
        repModel->addRubric(rubricStr);
    });

    removeRubricAction = new QAction("Удалить", this);
    removeRubricAction->setIcon(QIcon(":/icons/remove"));
    removeRubricAction->setDisabled(true);
    connect(removeRubricAction, &QAction::triggered, this, [this] {
        if (repView->selectionModel()->selectedRows().size() > 0)
            repModel->removeRubrics(repView->selectionModel()->selectedRows());
    });

    groupRubricsAction = new QAction("Группировать рубрики", this);
    removeRubricAction->setDisabled(true);
    connect(groupRubricsAction, &QAction::triggered, this, [this] {
        if (repView->selectionModel()->selectedRows().size() > 1)
            repModel->groupRubrics(repView->selectionModel()->selectedRows());
    });

    ungroupRubricsAction = new QAction("Разгруппировать рубрики", this);
    removeRubricAction->setDisabled(true);
    connect(ungroupRubricsAction, &QAction::triggered, this, [this] {
        if (repView->selectionModel()->selectedRows().size() == 1)
            repModel->ungroupRubrics(repView->selectionModel()->selectedRows().back());
    });

    changeImportanceActions = new QActionGroup(this);
    for (int i = 0; i <= 4; ++i) {
        QAction *action = new QAction(QString::number(i), changeImportanceActions);
        action->setData(i);
        action->setCheckable(true);
        changeImportanceActions->addAction(action);
        connect(action, &QAction::triggered, this, [this] {
            if (QAction *action = qobject_cast<QAction *>(sender())) {
                if (repView->selectionModel()->selectedRows().size() == 1)
                    repModel->setData(repView->selectionModel()->selectedRows().back(),
                                      action->data(),
                                      RepModel ::Roles::RubricImportance);
            }
        });
    }
    changeImportanceActions->setDisabled(true);
}

void Repertorizator::createMenu()
{
    repMenu = menuBar()->addMenu("Реперторизация");
    repMenu->addAction(newAction);
    repMenu->addAction(openAction);
    repMenu->addAction(saveAction);
    repMenu->addAction(saveAsAction);
    repMenu->addSeparator();
    repMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu("Редактирование");
    editMenu->addAction(addRubricAction);
    editMenu->addAction(removeRubricAction);

    contextMenu = new QMenu(this);
    contextMenu->addAction(addRubricAction);
    contextMenu->addAction(removeRubricAction);
    contextMenu->addSeparator();
    contextMenu->addAction(groupRubricsAction);
    contextMenu->addAction(ungroupRubricsAction);
}

void Repertorizator::createToolBar()
{
    toolBar = addToolBar("Инструменты");
    toolBar->setAllowedAreas(Qt::TopToolBarArea);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);

    toolBar->addAction(addRubricAction);
    toolBar->addAction(removeRubricAction);

    toolBar->addActions(changeImportanceActions->actions());
}

bool Repertorizator::readRep(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Предупреждение", "Не удалось открыть файл " + fileName);
        return false;
    }

    repModel->fromString(QString::fromUtf8(file.readAll()));

    currentRep = fileName;
    updateTitle(1);

    return true;
}

bool Repertorizator::writeRep(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Предупреждение", "Не удалось открыть файл " + fileName);
        return false;
    }

    QString repString = repModel->toString();
    if (file.write(repString.toUtf8()) == -1) {
        QMessageBox::warning(this, "Предупреждение", "Ошибка при записи в файл " + fileName);
        return false;
    }
    currentRep = fileName;
    updateTitle(1);

    return true;
}

void Repertorizator::newRep()
{
    if (!closeRep())
        return;
    currentRep.clear();
    repModel->clear();
    updateTitle(1);
}

void Repertorizator::openRep()
{
    if (!closeRep()) {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Выберите реперторизацию",
                                                    QString(),
                                                    "Реперторизации (*.rep)");
    if (fileName.isEmpty())
        return;

    readRep(fileName);
}

void Repertorizator::saveRep()
{
    QString fileName;
    if (currentRep.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this,
                                                "Сохранить",
                                                QString(),
                                                "Реперторизации (*.rep)");
        if (fileName.isEmpty())
            return;

    } else {
        fileName = currentRep;
    }

    writeRep(fileName);
}

void Repertorizator::saveRepAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить как",
                                                    QString(),
                                                    "Реперторизации (*.rep)");
    if (fileName.isEmpty())
        return;

    writeRep(fileName);
}

bool Repertorizator::closeRep()
{
    if (windowTitle().startsWith('*')) {
        QMessageBox::StandardButton answer = QMessageBox::question(this,
                                                                   "Несохранённые изменения",
                                                                   "Сохранить изменения?",
                                                                   QMessageBox::Yes
                                                                       | QMessageBox::No
                                                                       | QMessageBox::Cancel,
                                                                   QMessageBox::Yes);

        if (answer == QMessageBox::Cancel)
            return false;
        if (answer == QMessageBox ::Yes)
            saveRep();
    }
    return true;
}

void Repertorizator::showContextMenu(QPoint pos)
{
    contextMenu->popup(mapToGlobal(pos));
}

void Repertorizator::toggleActions()
{
    const auto &selectedRows = repView->selectionModel()->selectedRows();
    int selectedRowsCnt = selectedRows.size();

    removeRubricAction->setEnabled(selectedRowsCnt > 0);
    groupRubricsAction->setEnabled(selectedRowsCnt > 1);
    ungroupRubricsAction->setEnabled(selectedRowsCnt == 1
                                     && repModel->isParent(selectedRows.back()));
    changeImportanceActions->setEnabled(selectedRowsCnt == 1);
    if (selectedRowsCnt == 1) {
        auto importance = selectedRows.back().data(RepModel::Roles::RubricImportance).toUInt();
        changeImportanceActions->actions().at(importance)->setChecked(true);
    }
}

void Repertorizator::updateTitle(int state)
{
    QString currentTitle = windowTitle();
    switch (state) {
    case 1:
        setWindowTitle((currentRep.isEmpty() ? "Безымянная" : currentRep) + " - Реперторизатор");
        break;
    case 2:
        if (!currentTitle.startsWith('*'))
            setWindowTitle('*' + (currentRep.isEmpty() ? "Безымянная" : currentRep)
                           + " - Реперторизатор");
        break;
    }
}

void Repertorizator::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if (!closeRep()) {
        return;
    }
    event->accept();
}
