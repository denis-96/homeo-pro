#include "PatientCard.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>
#include "MultiLineEditDelegate.h"
#include "ui_PatientCard.h"

PatientCard::PatientCard(QWidget *parent)
    : HomeoEntity{parent}
    , ui{new Ui::PatientCard()}
{
    tabWidget = new QTabWidget(this);
    ui->setupUi(tabWidget);

    setupFields();

    auto layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

PatientCard::~PatientCard()
{
    delete ui;
}

bool PatientCard::read(QFile &file)
{
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject json = doc.object();

    for (const auto &child : tabWidget->findChildren<QWidget *>()) {
        QString name = child->objectName();
        if (!json.contains(name))
            continue;
        auto value = json[name];
        if (auto tEdit = dynamic_cast<QTextEdit *>(child)) {
            if (value.isString())
                tEdit->setHtml(value.toString());
        } else if (auto table = dynamic_cast<QTableWidget *>(child)) {
            if (!value.isArray())
                continue;

            QJsonArray rows = value.toArray();

            bool isDynamicTable = !table->rowCount();
            if (!isDynamicTable && rows.size() != table->rowCount())
                continue;

            for (int i = 0; i < rows.size(); ++i) {
                if (!rows[i].isArray())
                    continue;
                const QJsonArray cols = rows[i].toArray();
                if (cols.size() != table->columnCount())
                    continue;
                if (isDynamicTable)
                    table->insertRow(table->rowCount());
                for (int j = 0; j < table->columnCount(); ++j) {
                    table->setItem(i, j, new QTableWidgetItem(cols[j].toString()));
                }
            }
            table->resizeRowsToContents();

        } else if (auto lEdit = dynamic_cast<QLineEdit *>(child)) {
            if (value.isString())
                lEdit->setText(value.toString());
        } else if (auto dEdit = dynamic_cast<QDateEdit *>(child)) {
            if (value.isDouble())
                dEdit->setDate(QDate::fromJulianDay(value.toInteger()));
        } else if (auto nEdit = dynamic_cast<QSpinBox *>(child)) {
            if (value.isDouble())
                nEdit->setValue(value.toInt());
        } else if (auto tEdit = dynamic_cast<QPlainTextEdit *>(child)) {
            if (value.isString())
                tEdit->setPlainText(value.toString());
        }
    }

    return true;
}

bool PatientCard::write(QFile &file)
{
    QJsonObject json;

    for (const auto &child : tabWidget->findChildren<QWidget *>()) {
        if (auto tEdit = dynamic_cast<QTextEdit *>(child))
            json[child->objectName()] = tEdit->toHtml();
        else if (auto table = dynamic_cast<QTableWidget *>(child)) {
            QJsonArray tableJson;
            for (int i = 0; i < table->rowCount(); ++i) {
                QJsonArray row;
                for (int j = 0; j < table->columnCount(); ++j) {
                    row.push_back(table->item(i, j)->text());
                }
                tableJson.push_back(row);
            }
            json[table->objectName()] = tableJson;
        } else if (auto lEdit = dynamic_cast<QLineEdit *>(child))
            json[child->objectName()] = lEdit->text();
        else if (auto dEdit = dynamic_cast<QDateEdit *>(child))
            json[child->objectName()] = dEdit->date().toJulianDay();
        else if (auto nEdit = dynamic_cast<QSpinBox *>(child))
            json[child->objectName()] = nEdit->value();
        else if (auto tEdit = dynamic_cast<QPlainTextEdit *>(child))
            json[child->objectName()] = tEdit->toPlainText();
    }

    return file.write(QJsonDocument(json).toJson());
}

void PatientCard::setupFields()
{
    for (const auto &child : tabWidget->findChildren<QWidget *>()) {
        if (auto castedEdit = dynamic_cast<QTextEdit *>(child)) {
            connect(castedEdit, &QTextEdit::textChanged, this, &HomeoEntity::changed);
            setupTextEdit(castedEdit);
        } else if (auto table = dynamic_cast<QTableWidget *>(child)) {
            connect(table, &QTableWidget::cellChanged, this, &HomeoEntity::changed);
            table->setItemDelegate(new MultiLineEditDelegate(table));
            table->horizontalHeader()->setStretchLastSection(true);
            table->horizontalHeader()->setDefaultSectionSize(200);
        } else if (auto castedEdit = dynamic_cast<QLineEdit *>(child))
            connect(castedEdit, &QLineEdit::editingFinished, this, &HomeoEntity ::changed);
        else if (auto castedEdit = dynamic_cast<QDateEdit *>(child))
            connect(castedEdit, &QDateEdit::editingFinished, this, &HomeoEntity ::changed);
        else if (auto castedEdit = dynamic_cast<QSpinBox *>(child))
            connect(castedEdit, &QSpinBox::editingFinished, this, &HomeoEntity ::changed);
        else if (auto castedEdit = dynamic_cast<QPlainTextEdit *>(child))
            connect(castedEdit, &QPlainTextEdit::textChanged, this, &HomeoEntity::changed);
    }

    setupEditableTable(ui->complaints);
    setupEditableTable(ui->diary);

    ui->edit4->setDate(QDate::currentDate());
    ui->edit4->setMinimumDate(QDate(2000, 1, 1));
    ui->edit4->setMaximumDate(QDate::currentDate());

    ui->edit162->setFixedHeight(400);
}

void PatientCard::setupEditableTable(QTableWidget *table)
{
    auto addRowAction = new QAction(QIcon(":/resources/icons/add.svg"), "Добавить запись", table);
    connect(addRowAction, &QAction::triggered, table, [table] {
        int row = table->rowCount();
        table->insertRow(row);
        for (int col = 0; col < table->columnCount(); ++col) {
            table->setItem(row, col, new QTableWidgetItem);
        }
    });
    table->addAction(addRowAction);

    auto removeRowAction = new QAction(QIcon(":/resources/icons/remove.svg"),
                                       "Удалить запись",
                                       table);
    removeRowAction->setDisabled(true);
    connect(removeRowAction, &QAction::triggered, table, [table] {
        table->removeRow(table->selectionModel()->selectedRows().back().row());
    });
    table->addAction(removeRowAction);

    table->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(table->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            table,
            [table, removeRowAction] {
                removeRowAction->setEnabled(table->selectionModel()->selectedRows().size());
            });
}

void PatientCard::setupTextEdit(QTextEdit *edit)
{
    auto highlightAction = new QAction("Выделить");
    connect(highlightAction, &QAction::triggered, edit, [edit]() {
        QTextCursor cursor = edit->textCursor();

        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);

        QTextCharFormat fmt;
        fmt.setBackground(cursor.charFormat().background().color() != QColor(Qt::green)
                              ? Qt::green
                              : fmt.background());

        // fmt.setFontWeight(cursor.charFormat().font().bold() ? QFont::Normal : QFont::Bold);

        cursor.mergeCharFormat(fmt);
    });
    edit->addAction(highlightAction);
    auto separator = new QAction;
    separator->setSeparator(true);
    edit->addAction(separator);
    edit->addActions(edit->createStandardContextMenu()->actions());
    edit->setContextMenuPolicy(Qt::ActionsContextMenu);
    edit->setFixedHeight(70);
}
