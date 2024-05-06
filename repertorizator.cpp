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

    connect(repView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &Repertorizator::toggleRepActions);
    connect(repModel, &RepModel::dataChanged, this, &Repertorizator::onRepChange);
    this->setCentralWidget(repView);
    this->setWindowIcon(QIcon(":/icons/main-icon"));
}

void Repertorizator::createActions()
{
    newAction = new QAction("Новая", this);
    newAction->setIcon(QIcon(":/icons/new-file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newRep()));

    openAction = new QAction("Открыть", this);
    openAction->setIcon(QIcon(":/icons/open-file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openRep()));

    saveAction = new QAction("Сохранить", this);
    saveAction->setIcon(QIcon(":/icons/save-file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveRep()));

    saveAsAction = new QAction("Сохранить как", this);
    saveAsAction->setIcon(QIcon(":/icons/save-file"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveRepAs()));

    exitAction = new QAction("Выход", this);
    exitAction->setIcon(QIcon(":/icons/exit"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    addRubricAction = new QAction("Добавить", this);
    addRubricAction->setIcon(QIcon(":/icons/add"));
    connect(addRubricAction, SIGNAL(triggered()), this, SLOT(addRubric()));

    removeRubricAction = new QAction("Удалить", this);
    removeRubricAction->setIcon(QIcon(":/icons/remove"));
    removeRubricAction->setDisabled(true);
    connect(removeRubricAction, SIGNAL(triggered()), this, SLOT(removeRubric()));

    toggleRubricAction = new QAction("Вкл/Выкл", this);
    toggleRubricAction->setIcon(QIcon(":/icons/toggle"));
    toggleRubricAction->setDisabled(true);
    // connect(toggleRubricAction, SIGNAL(triggered()), this, SLOT(toggleRubric()));
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
    editMenu->addAction(toggleRubricAction);
}

void Repertorizator::createToolBar()
{
    toolBar = addToolBar("Инструменты");
    toolBar->setAllowedAreas(Qt::TopToolBarArea);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);

    toolBar->addAction(addRubricAction);
    toolBar->addAction(removeRubricAction);
    toolBar->addAction(toggleRubricAction);
}

bool Repertorizator::readRep(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Предупреждение", "Не удалось открыть файл " + fileName);
        return false;
    }

    repModel->fromString(QString::fromUtf8(file.readAll()));

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

    if (!readRep(fileName))
        return;

    currentRep = fileName;
    updateTitle(1);
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

    if (!writeRep(fileName))
        return;

    currentRep = fileName;
    updateTitle(1);
}

void Repertorizator::saveRepAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить как",
                                                    QString(),
                                                    "Реперторизации (*.rep)");
    if (fileName.isEmpty())
        return;

    if (!writeRep(fileName))
        return;

    currentRep = fileName;
    updateTitle(1);
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

void Repertorizator::addRubric()
{
    QString rubricStr = QApplication::clipboard()->text();
    repModel->addRubric(rubricStr);
}

void Repertorizator::removeRubric()
{
    QModelIndexList selectedRows = repView->selectionModel()->selectedRows();
    for (const auto &row : selectedRows) {
        repModel->removeRubric(row);
    }

    toggleRepActions();
}

void Repertorizator::onRepChange()
{
    updateTitle(2);
}

void Repertorizator::toggleRepActions()
{
    QModelIndexList selectedRows = repView->selectionModel()->selectedRows();
    removeRubricAction->setEnabled((bool) selectedRows.size());
    toggleRubricAction->setEnabled((bool) selectedRows.size());
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

void Repertorizator::initialize()
{
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() > 1 && QFile::exists(arguments.last())) {
        if (/*!repModel->loadFromRepFile(arguments.last())*/ true) {
            QMessageBox::warning(this,
                                 "Предупреждение",
                                 "Не удалось открыть файл " + arguments.last());
            return;
        }
        currentRep = arguments.last();
        updateTitle(1);
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
