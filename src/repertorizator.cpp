#include "repertorizator.h"
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QListView>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>

Repertorizator::Repertorizator(QWidget *parent)
    : QMainWindow(parent)
{
    updateTitle(1);
    resize(900, 500);

    repModel = new RepModel(this);
    repView = new RepView(this);
    repProxyModel = new RepProxyModel(this);
    rubricsView = new QListView(this);

    repView->setRepModel(repModel);
    repProxyModel->setSourceModel(repModel);
    rubricsView->setModel(repProxyModel);
    rubricsView->setSelectionMode(QAbstractItemView::MultiSelection);
    rubricsView->setSelectionBehavior(QAbstractItemView::SelectRows);

    setupUI();
}
Repertorizator::~Repertorizator() {}

void Repertorizator::setupUI()
{
    createActions();
    createMenu();
    createToolBar();
    createDialogs();

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
    this->setWindowIcon(QIcon(":/resources/icons/main-icon.svg"));

    setFont(QFont("Segoe UI", 12));
    auto pal = palette();
    pal.setColor(QPalette::Base, QColor(0xD2D2D2));
    setPalette(pal);
}

void Repertorizator::createActions()
{
    newAction = new QAction("Новая", this);
    newAction->setIcon(QIcon(":/resources/icons/new-file.svg"));
    connect(newAction, &QAction::triggered, this, &Repertorizator::newRep);

    openAction = new QAction("Открыть", this);
    openAction->setIcon(QIcon(":/resources/icons/open-file.svg"));
    connect(openAction, &QAction::triggered, this, &Repertorizator::openRep);

    saveAction = new QAction("Сохранить", this);
    saveAction->setIcon(QIcon(":/resources/icons/save-file.svg"));
    connect(saveAction, &QAction::triggered, this, &Repertorizator::saveRep);

    saveAsAction = new QAction("Сохранить как", this);
    saveAsAction->setIcon(QIcon(":/resources/icons/save-file.svg"));
    connect(saveAsAction, &QAction::triggered, this, &Repertorizator::saveRepAs);

    exitAction = new QAction("Выход", this);
    exitAction->setIcon(QIcon(":/resources/icons/exit.svg"));
    connect(exitAction, &QAction::triggered, this, &Repertorizator::close);

    addRubricAction = new QAction("Добавить", this);
    addRubricAction->setIcon(QIcon(":/resources/icons/add.svg"));
    connect(addRubricAction, &QAction::triggered, this, &Repertorizator::addRubric);

    removeRubricAction = new QAction("Удалить", this);
    removeRubricAction->setIcon(QIcon(":/resources/icons/remove.svg"));
    removeRubricAction->setDisabled(true);
    connect(removeRubricAction, &QAction::triggered, this, &Repertorizator::removeRubric);

    groupRubricsAction = new QAction("Группировать рубрики", this);
    connect(groupRubricsAction, &QAction::triggered, this, &Repertorizator::groupRubrics);

    ungroupRubricsAction = new QAction("Разгруппировать рубрики", this);
    ungroupRubricsAction->setDisabled(true);
    connect(ungroupRubricsAction, &QAction::triggered, this, &Repertorizator::ungroupRubrics);

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

    // remove after beta-testing
    repMenu->addSeparator();
    auto openOld = new QAction("Открыть(пред. версия)", this);
    connect(openOld, &QAction::triggered, this, [this] {
        if (!closeRep()) {
            return;
        }
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        "Выберите реперторизацию",
                                                        QString(),
                                                        "Реперторизации (*.rep)");
        if (fileName.isEmpty())
            return;

        QFile file1(fileName);
        if (!file1.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file1);
        int dCount;
        in >> dCount;

        std::map<int, QString> drugs;
        for (int i = 0; i < dCount; ++i) {
            int id;
            in >> id;
            in.readLine();
            QString title = in.readLine();
            title.replace(" ", "");
            drugs.insert({id, title});
        }
        int rCount;
        in >> rCount;

        std::vector<std::pair<QString, std::vector<std::pair<QString, int>>>> rubrics;
        for (int i = 0; i < rCount; ++i) {
            in.readLine();
            QString title = in.readLine();
            int s;
            int dC;
            in >> s >> dC;
            rubrics.push_back({title, {}});
            for (int j = 0; j < dC; ++j) {
                int dId, degree;
                in >> dId >> degree;
                rubrics.back().second.push_back({drugs[dId], degree});
            }
        }

        file1.close();

        QFile file2(fileName);
        if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file2);
        for (const auto &r : rubrics) {
            out << r.first << ":";
            for (const auto &d : r.second) {
                out << " " << d.first << "(" << d.second << ")";
            }
            out << ".\n";
        }
        file2.close();

        readRep(fileName);
    });
    repMenu->addAction(openOld);
    //

    editMenu = menuBar()->addMenu("Редактирование");
    editMenu->addAction(addRubricAction);
    editMenu->addAction(removeRubricAction);

    contextMenu = new QMenu(this);
    contextMenu->addAction(addRubricAction);
    contextMenu->addAction(removeRubricAction);
    contextMenu->addSeparator();
    contextMenu->addAction(groupRubricsAction);
    contextMenu->addAction(ungroupRubricsAction);
    contextMenu->addMenu("Значимость")->addActions(changeImportanceActions->actions());
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

void Repertorizator::createDialogs()
{
    groupCreationDialog = new QDialog(this);

    QLineEdit *titleEdit = new QLineEdit(groupCreationDialog);
    titleEdit->setText("Объединение рубрик");
    connect(rubricsView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            titleEdit,
            [titleEdit, this] {
                titleEdit->setText(
                    generateTitleForGroup(rubricsView->selectionModel()->selectedRows()));
            });

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel,
                                                       groupCreationDialog);
    connect(buttonBox, &QDialogButtonBox::accepted, groupCreationDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, groupCreationDialog, &QDialog::reject);

    auto layout = new QVBoxLayout(groupCreationDialog);
    layout->addWidget(rubricsView);
    layout->addWidget(titleEdit);
    layout->addWidget(buttonBox);

    groupCreationDialog->setLayout(layout);
}

QString Repertorizator::generateTitleForGroup(const QModelIndexList &rubrics)
{
    if (rubrics.empty())
        return "Объединение рубрик";
    int minSize = 100000000;
    QStringList titles;
    titles.reserve(rubrics.size());

    for (const auto &rub : rubrics) {
        titles.push_back(rub.data(Qt::UserRole).toString());
        if (titles.back().size() < minSize)
            minSize = titles.back().size();
    }
    int endEqual = 0;
    for (int i = 0; i < minSize; ++i) {
        bool equal = true;
        QChar character;
        for (const auto &title : titles) {
            if (character.isNull())
                character = title[i];
            if (character != title[i]) {
                equal = false;
                break;
            }
        }
        endEqual = i;
        if (!equal) {
            break;
        }
    }
    if (endEqual < 2)
        return "Объединение рубрик";
    return titles.back().left(endEqual).trimmed();
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

void Repertorizator::addRubric()
{
    QString rubricStr = QApplication::clipboard()->text();
    auto idx = repModel->addRubric(rubricStr);
    if (!idx.isValid())
        QMessageBox::information(this, "Добавление рубрики", "Введённая рубрика уже существует!");
}

void Repertorizator::removeRubric()
{
    if (repView->selectionModel()->selectedRows().size() > 0)
        repModel->removeRubrics(repView->selectionModel()->selectedRows());
}

void Repertorizator::groupRubrics()
{
    QString title;
    QModelIndexList selectedRubrics;

    if (repView->selectionModel()->selectedRows().size() > 1) {
        bool ok;
        selectedRubrics = repView->selectionModel()->selectedRows();
        title = QInputDialog::getText(this,
                                      "Группирование рубрик",
                                      "Название группы:",
                                      QLineEdit::Normal,
                                      generateTitleForGroup(selectedRubrics),
                                      &ok);
        if (!ok)
            return;

    } else if (groupCreationDialog->exec() == QDialog::Accepted) {
        title = ((QLineEdit *) groupCreationDialog->layout()->itemAt(1)->widget())->text();
        selectedRubrics = rubricsView->selectionModel()->selectedRows();
        for (auto &r : selectedRubrics) {
            r = repProxyModel->mapToSource(r);
        }
    }
    if (!title.isEmpty() && !selectedRubrics.empty()) {
        auto idx = repModel->groupRubrics(selectedRubrics, title);
        if (!idx.isValid())
            QMessageBox::information(this,
                                     "Группирование рубрик",
                                     QString("Группа с названием \"%1\" уже существует!").arg(title));
    }
}

void Repertorizator::ungroupRubrics()
{
    if (repView->selectionModel()->selectedRows().size() == 1)
        repModel->ungroupRubrics(repView->selectionModel()->selectedRows().back());
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
    groupRubricsAction->setText(selectedRowsCnt > 1 ? "Группировать выбранное"
                                                    : "Группировать рубрики");

    changeImportanceActions->setEnabled(selectedRowsCnt == 1);

    ungroupRubricsAction->setEnabled(selectedRowsCnt == 1
                                     && repModel->hasChildren(selectedRows.back()));

    if (selectedRowsCnt == 1) {
        auto importance = selectedRows.back().data(RepModel::Roles::RubricImportance).toUInt();
        changeImportanceActions->actions().at(importance)->setChecked(true);
    }
}

void Repertorizator::openWith()
{
    QStringList arguments = qApp->arguments();
    if (arguments.size() > 1 && QFile::exists(arguments.back())) {
        auto fileName = arguments.last();
        readRep(fileName);
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
