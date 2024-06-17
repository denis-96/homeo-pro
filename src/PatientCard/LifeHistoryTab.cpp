/*
#include "LifeHistoryTab.h"
#include <QHeaderView>
#include <QJsonValue>
#include <QLabel>
#include <QVBoxLayout>
#include "MultiLineEditDelegate.h"

LifeHistoryTab::LifeHistoryTab(const QString &title, QWidget *parent)
    : PCardTab(title, parent)

{
    auto layout = new QVBoxLayout(this);
    for (const auto &label : labels) {
        auto questionLbl = new QLabel(label, this);
        questionLbl->setWordWrap(true);
        auto answerEdit = new QPlainTextEdit(this);
        auto f = answerEdit->font();
        f.setPointSize(14);
        answerEdit->setFont(f);

        connect(answerEdit, &QPlainTextEdit::textChanged, this, &PCardTab::changed);

        answerEdits.push_back(answerEdit);

        layout->addWidget(questionLbl);
        layout->addWidget(answerEdit);
        layout->addSpacing(20);
    }
    auto diseasesLbl = new QLabel(diseasesLabel, this);
    diseasesLbl->setWordWrap(true);
    layout->addWidget(diseasesLbl);

    diseasesModel = new DiseasesModel(this);
    connect(diseasesModel, &QAbstractItemModel::dataChanged, this, &PCardTab::changed);
    connect(diseasesModel, &QAbstractItemModel::rowsInserted, this, &PCardTab::changed);
    connect(diseasesModel, &QAbstractItemModel::rowsRemoved, this, &PCardTab::changed);

    diseasesView = new QTableView(this);
    diseasesView->setModel(diseasesModel);
    diseasesView->setColumnWidth(0, 300);
    diseasesView->setWordWrap(true);
    diseasesView->horizontalHeader()->setStretchLastSection(true);
    diseasesView->verticalHeader()->setDefaultSectionSize(60);
    diseasesView->setSelectionMode(QAbstractItemView::SingleSelection);
    diseasesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    diseasesView->setItemDelegate(new MultiLineEditDelegate(this));

    addDiseaseAction = new QAction(QIcon(":/resources/icons/add.svg"), "Добавить запись", this);
    connect(addDiseaseAction, &QAction::triggered, this, [this] { diseasesModel->appendRow(); });
    diseasesView->addAction(addDiseaseAction);

    removeDiseaseAction = new QAction(QIcon(":/resources/icons/remove.svg"), "Удалить запись", this);
    removeDiseaseAction->setDisabled(true);
    connect(removeDiseaseAction, &QAction::triggered, this, [this] {
        diseasesModel->removeRow(diseasesView->selectionModel()->selectedRows().back());
    });
    diseasesView->addAction(removeDiseaseAction);
    diseasesView->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(diseasesView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this] {
        removeDiseaseAction->setEnabled(diseasesView->selectionModel()->selectedRows().size());
    });

    layout->addWidget(diseasesView);

    setLayout(layout);
}

void LifeHistoryTab::read(const QJsonValue &json) {}

QJsonValue LifeHistoryTab::toJson() const {}
*/
