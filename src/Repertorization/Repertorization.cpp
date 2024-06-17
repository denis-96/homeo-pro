#include "Repertorization.h"
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

Repertorization::Repertorization(QWidget *parent)
    : HomeoEntity{parent}
{
    repModel = new RepModel(this);
    repView = new RepView(this);
    repProxyModel = new RepProxyModel(this);
    rubricsView = new QListView(this);

    repView->setModel(repModel);
    repProxyModel->setSourceModel(repModel);
    rubricsView->setModel(repProxyModel);
    rubricsView->setSelectionMode(QAbstractItemView::MultiSelection);
    rubricsView->setSelectionBehavior(QAbstractItemView::SelectRows);

    setupUI();
}

bool Repertorization::read(QFile &file)
{
    repModel->read(QString::fromUtf8(file.readAll()));
    return true;
}

bool Repertorization::write(QFile &file)
{
    QString repString = repModel->toString();
    return (file.write(repString.toUtf8()) != -1);
}

void Repertorization::setupUI()
{
    createActions();
    createContextMenu();
    createDialogs();

    addAction(addRubricAction);
    addAction(removeRubricAction);
    addActions(changeImportanceActions->actions());

    toolBar = new QToolBar("Инструменты", this);
    toolBar->setAllowedAreas(Qt::TopToolBarArea);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    toolBar->addAction(addRubricAction);
    toolBar->addAction(removeRubricAction);
    toolBar->addActions(changeImportanceActions->actions());

    auto layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(toolBar);
    layout->addWidget(repView);
    setLayout(layout);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,
            &Repertorization::customContextMenuRequested,
            this,
            &Repertorization::showContextMenu);

    connect(repView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &Repertorization::updateActions);

    setFont(QFont("Segoe UI", 12));
}

void Repertorization::createActions()
{
    addRubricAction = new QAction("Добавить", this);
    addRubricAction->setIcon(QIcon(":/resources/icons/add.svg"));
    connect(addRubricAction, &QAction::triggered, this, &Repertorization::addRubric);

    removeRubricAction = new QAction("Удалить", this);
    removeRubricAction->setIcon(QIcon(":/resources/icons/remove.svg"));
    removeRubricAction->setDisabled(true);
    connect(removeRubricAction, &QAction::triggered, this, &Repertorization::removeRubric);

    groupRubricsAction = new QAction("Группировать рубрики", this);
    connect(groupRubricsAction, &QAction::triggered, this, &Repertorization::groupRubrics);

    ungroupRubricsAction = new QAction("Разгруппировать рубрики", this);
    ungroupRubricsAction->setDisabled(true);
    connect(ungroupRubricsAction, &QAction::triggered, this, &Repertorization::ungroupRubrics);

    changeImportanceActions = new QActionGroup(this);
    for (int i = 0; i <= 4; ++i) {
        QAction *action = new QAction(QString::number(i), changeImportanceActions);
        action->setData(i);
        action->setCheckable(true);
        changeImportanceActions->addAction(action);
        connect(action, &QAction::triggered, this, &Repertorization::changeRubricImportance);
    }
    changeImportanceActions->setDisabled(true);
}

void Repertorization::createContextMenu()
{
    contextMenu = new QMenu(this);
    contextMenu->addAction(addRubricAction);
    contextMenu->addAction(removeRubricAction);
    contextMenu->addSeparator();
    contextMenu->addAction(groupRubricsAction);
    contextMenu->addAction(ungroupRubricsAction);
    contextMenu->addMenu("Значимость")->addActions(changeImportanceActions->actions());
}

void Repertorization::createDialogs()
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

QString Repertorization::generateTitleForGroup(const QModelIndexList &rubrics)
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

void Repertorization::addRubric()
{
    QString rubricStr = QApplication::clipboard()->text();
    auto idx = repModel->addRubric(rubricStr);
    if (!idx.isValid())
        QMessageBox::information(this, "Добавление рубрики", "Введённая рубрика уже существует!");
    else
        emit changed();
}

void Repertorization::removeRubric()
{
    if (repView->selectionModel()->selectedRows().size() > 0) {
        repModel->removeRubrics(repView->selectionModel()->selectedRows());
        emit changed();
    }
}

void Repertorization::groupRubrics()
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
        title = dynamic_cast<QLineEdit *>(groupCreationDialog->layout()->itemAt(1)->widget())->text();
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
        else
            emit changed();
    }
}

void Repertorization::ungroupRubrics()
{
    if (repView->selectionModel()->selectedRows().size() == 1) {
        repModel->ungroupRubrics(repView->selectionModel()->selectedRows().back());
        emit changed();
    }
}

void Repertorization::changeRubricImportance()
{
    if (QAction *action = qobject_cast<QAction *>(sender())) {
        if (repView->selectionModel()->selectedRows().size() == 1) {
            repModel->setData(repView->selectionModel()->selectedRows().back(),
                              action->data(),
                              RepModel ::Roles::RubricImportance);
        }
    }
}

void Repertorization::showContextMenu(QPoint pos)
{
    contextMenu->popup(mapToGlobal(pos));
}

void Repertorization::updateActions()
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
