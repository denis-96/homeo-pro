#include "PatientCard.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollArea>
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

    if (!json["edits"].isArray())
        return false;
    QJsonArray editsJson = json["edits"].toArray();

    if (editsJson.size() == edits.size())
        for (int i = 0; i < edits.size(); ++i) {
            auto edit = edits[i];
            auto value = editsJson[i];
            if (auto tEdit = dynamic_cast<QPlainTextEdit *>(edit)) {
                if (value.isString())
                    tEdit->setPlainText(value.toString());
            } else if (auto lEdit = dynamic_cast<QLineEdit *>(edit)) {
                if (value.isString())
                    lEdit->setText(value.toString());
            } else if (auto dEdit = dynamic_cast<QDateEdit *>(edit)) {
                if (value.isDouble())
                    dEdit->setDate(QDate::fromJulianDay(value.toInteger()));
            } else if (auto nEdit = dynamic_cast<QSpinBox *>(edit)) {
                if (value.isDouble())
                    nEdit->setValue(value.toInt());
            }
        }

    if (!json["tables"].isArray())
        return false;
    QJsonArray tablesJson = json["tables"].toArray();

    if (tablesJson.size() == tables.size())
        for (int i = 0; i < tables.size(); ++i) {
            if (!tablesJson[i].isArray())
                continue;
            auto table = tables[i];
            QJsonArray value = tablesJson[i].toArray();

            bool isDynamicTable = !table->rowCount();
            if (!isDynamicTable && value.size() != table->rowCount())
                continue;

            for (int i = 0; i < value.size(); ++i) {
                if (!value[i].isArray())
                    continue;
                const QJsonArray cols = value[i].toArray();
                if (cols.size() != table->columnCount())
                    continue;
                if (isDynamicTable)
                    table->insertRow(table->rowCount());
                for (int j = 0; j < table->columnCount(); ++j) {
                    table->setItem(i, j, new QTableWidgetItem(cols[j].toString()));
                }
            }
            table->resizeRowsToContents();
        }

    return true;
}

bool PatientCard::write(QFile &file)
{
    QJsonObject json;

    QJsonArray editsJson;
    for (const auto &edit : edits) {
        if (auto tEdit = dynamic_cast<QPlainTextEdit *>(edit))
            editsJson.push_back(tEdit->toPlainText());
        else if (auto lEdit = dynamic_cast<QLineEdit *>(edit))
            editsJson.push_back(lEdit->text());
        else if (auto dEdit = dynamic_cast<QDateEdit *>(edit))
            editsJson.push_back(dEdit->date().toJulianDay());
        else if (auto nEdit = dynamic_cast<QSpinBox *>(edit))
            editsJson.push_back(nEdit->value());
    }

    QJsonArray tablesJson;
    for (const auto &table : tables) {
        QJsonArray tableJson;
        for (int i = 0; i < table->rowCount(); ++i) {
            QJsonArray row;
            for (int j = 0; j < table->columnCount(); ++j) {
                row.push_back(table->item(i, j)->text());
            }
            tableJson.push_back(row);
        }
        tablesJson.push_back(tableJson);
    }

    json["edits"] = editsJson;
    json["tables"] = tablesJson;

    return file.write(QJsonDocument(json).toJson());
}

void PatientCard::setupFields()
{
    edits.push_back(ui->edit1);
    edits.push_back(ui->edit2);
    edits.push_back(ui->edit3);
    edits.push_back(ui->edit4);
    edits.push_back(ui->edit5);
    edits.push_back(ui->edit6);
    edits.push_back(ui->edit7);
    edits.push_back(ui->edit8);
    edits.push_back(ui->edit9);
    edits.push_back(ui->edit10);
    edits.push_back(ui->edit11);
    edits.push_back(ui->edit12);
    edits.push_back(ui->edit13);
    edits.push_back(ui->edit14);
    edits.push_back(ui->edit15);
    edits.push_back(ui->edit16);
    edits.push_back(ui->edit17);
    edits.push_back(ui->edit18);
    edits.push_back(ui->edit19);
    edits.push_back(ui->edit20);
    edits.push_back(ui->edit21);
    edits.push_back(ui->edit22);
    edits.push_back(ui->edit23);
    edits.push_back(ui->edit24);
    edits.push_back(ui->edit25);
    edits.push_back(ui->edit26);
    edits.push_back(ui->edit27);
    edits.push_back(ui->edit28);
    edits.push_back(ui->edit29);
    edits.push_back(ui->edit30);
    edits.push_back(ui->edit31);
    edits.push_back(ui->edit32);
    edits.push_back(ui->edit33);
    edits.push_back(ui->edit34);
    edits.push_back(ui->edit35);
    edits.push_back(ui->edit36);
    edits.push_back(ui->edit37);
    edits.push_back(ui->edit38);
    edits.push_back(ui->edit39);
    edits.push_back(ui->edit40);
    edits.push_back(ui->edit41);
    edits.push_back(ui->edit42);
    edits.push_back(ui->edit43);
    edits.push_back(ui->edit44);
    edits.push_back(ui->edit45);
    edits.push_back(ui->edit46);
    edits.push_back(ui->edit47);
    edits.push_back(ui->edit48);
    edits.push_back(ui->edit49);
    edits.push_back(ui->edit50);
    edits.push_back(ui->edit51);
    edits.push_back(ui->edit52);
    edits.push_back(ui->edit53);
    edits.push_back(ui->edit54);
    edits.push_back(ui->edit55);
    edits.push_back(ui->edit56);
    edits.push_back(ui->edit57);
    edits.push_back(ui->edit58);
    edits.push_back(ui->edit59);
    edits.push_back(ui->edit60);
    edits.push_back(ui->edit61);
    edits.push_back(ui->edit62);
    edits.push_back(ui->edit63);
    edits.push_back(ui->edit64);
    edits.push_back(ui->edit65);
    edits.push_back(ui->edit66);
    edits.push_back(ui->edit67);
    edits.push_back(ui->edit68);
    edits.push_back(ui->edit69);
    edits.push_back(ui->edit70);
    edits.push_back(ui->edit71);
    edits.push_back(ui->edit72);
    edits.push_back(ui->edit73);
    edits.push_back(ui->edit74);
    edits.push_back(ui->edit75);
    edits.push_back(ui->edit76);
    edits.push_back(ui->edit77);
    edits.push_back(ui->edit78);
    edits.push_back(ui->edit79);
    edits.push_back(ui->edit80);
    edits.push_back(ui->edit81);
    edits.push_back(ui->edit82);
    edits.push_back(ui->edit83);
    edits.push_back(ui->edit84);
    edits.push_back(ui->edit85);
    edits.push_back(ui->edit86);
    edits.push_back(ui->edit87);
    edits.push_back(ui->edit88);
    edits.push_back(ui->edit89);
    edits.push_back(ui->edit90);
    edits.push_back(ui->edit91);
    edits.push_back(ui->edit92);
    edits.push_back(ui->edit93);
    edits.push_back(ui->edit94);
    edits.push_back(ui->edit95);
    edits.push_back(ui->edit96);
    edits.push_back(ui->edit97);
    edits.push_back(ui->edit98);
    edits.push_back(ui->edit99);
    edits.push_back(ui->edit100);
    edits.push_back(ui->edit101);
    edits.push_back(ui->edit102);
    edits.push_back(ui->edit103);
    edits.push_back(ui->edit104);
    edits.push_back(ui->edit105);
    edits.push_back(ui->edit106);
    edits.push_back(ui->edit107);
    edits.push_back(ui->edit108);
    edits.push_back(ui->edit109);
    edits.push_back(ui->edit110);
    edits.push_back(ui->edit111);
    edits.push_back(ui->edit112);
    edits.push_back(ui->edit113);
    edits.push_back(ui->edit114);
    edits.push_back(ui->edit115);
    edits.push_back(ui->edit116);
    edits.push_back(ui->edit117);
    edits.push_back(ui->edit118);
    edits.push_back(ui->edit119);
    edits.push_back(ui->edit120);
    edits.push_back(ui->edit121);
    edits.push_back(ui->edit122);
    edits.push_back(ui->edit123);
    edits.push_back(ui->edit124);
    edits.push_back(ui->edit125);
    edits.push_back(ui->edit126);
    edits.push_back(ui->edit127);
    edits.push_back(ui->edit128);
    edits.push_back(ui->edit129);
    edits.push_back(ui->edit130);
    edits.push_back(ui->edit131);
    edits.push_back(ui->edit132);
    edits.push_back(ui->edit133);
    edits.push_back(ui->edit134);
    edits.push_back(ui->edit135);
    edits.push_back(ui->edit136);
    edits.push_back(ui->edit137);
    edits.push_back(ui->edit138);
    edits.push_back(ui->edit139);
    edits.push_back(ui->edit140);
    edits.push_back(ui->edit141);
    edits.push_back(ui->edit142);
    edits.push_back(ui->edit143);
    edits.push_back(ui->edit144);
    edits.push_back(ui->edit145);
    edits.push_back(ui->edit146);
    edits.push_back(ui->edit147);
    edits.push_back(ui->edit148);
    edits.push_back(ui->edit149);
    edits.push_back(ui->edit150);
    edits.push_back(ui->edit151);
    edits.push_back(ui->edit152);
    edits.push_back(ui->edit153);
    edits.push_back(ui->edit154);
    edits.push_back(ui->edit155);
    edits.push_back(ui->edit156);
    edits.push_back(ui->edit157);
    edits.push_back(ui->edit158);
    edits.push_back(ui->edit159);
    edits.push_back(ui->edit160);
    edits.push_back(ui->edit161);
    edits.push_back(ui->edit162);
    edits.push_back(ui->edit163);
    edits.push_back(ui->edit164);
    edits.push_back(ui->edit165);
    edits.push_back(ui->edit166);
    edits.push_back(ui->edit167);
    edits.push_back(ui->edit168);
    edits.push_back(ui->edit169);
    edits.push_back(ui->edit170);
    edits.push_back(ui->edit171);
    edits.push_back(ui->edit172);
    edits.push_back(ui->edit173);
    edits.push_back(ui->edit174);
    edits.push_back(ui->edit175);
    edits.push_back(ui->edit176);
    edits.push_back(ui->edit177);
    edits.push_back(ui->edit178);

    tables.push_back(ui->complaints);
    tables.push_back(ui->table1);
    tables.push_back(ui->table2);
    tables.push_back(ui->table3);
    tables.push_back(ui->table4);
    tables.push_back(ui->diary);

    for (const auto &edit : edits) {
        if (auto castedEdit = dynamic_cast<QPlainTextEdit *>(edit))
            connect(castedEdit, &QPlainTextEdit::textChanged, this, &HomeoEntity::changed);
        else if (auto castedEdit = dynamic_cast<QLineEdit *>(edit))
            connect(castedEdit, &QLineEdit::editingFinished, this, &HomeoEntity ::changed);
        else if (auto castedEdit = dynamic_cast<QDateEdit *>(edit))
            connect(castedEdit, &QDateEdit::editingFinished, this, &HomeoEntity ::changed);
        else if (auto castedEdit = dynamic_cast<QSpinBox *>(edit))
            connect(castedEdit, &QSpinBox::editingFinished, this, &HomeoEntity ::changed);
    }
    for (const auto &table : tables) {
        connect(table, &QTableWidget::cellChanged, this, &HomeoEntity::changed);
        table->setItemDelegate(new MultiLineEditDelegate(table));
    }

    setupEditableTable(ui->complaints);
    setupEditableTable(ui->diary);

    ui->edit4->setDate(QDate::currentDate());
    ui->edit4->setMinimumDate(QDate(2000, 1, 1));
    ui->edit4->setMaximumDate(QDate::currentDate());
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
/*
void convert(const std::pair<QString, PCardTab *> &val)
{
    static int editCnt = 0;
    static int labelCnt = 0;
    static int tableCnt = 0;

    QFile file("C:\\Users\\denis\\Desktop\\result.xml");
    file.open(QIODevice::Append);
    QTextStream ts(&file);
    if (CurrentStateTab *cst = dynamic_cast<CurrentStateTab *>(val.second)) {
        ts << QString("<widget class=\"QTabWidget\" name=\"%1\"><property "
                      "name=\"tabPosition\"><enum>QTabWidget::West</enum></property><attribute "
                      "name=\"title\"><string>%2</string></attribute>")
                  .arg(val.first)
                  .arg(cst->title());
        int tCnt = 1;
        for (const auto &st : cst->content) {
            ts << QString("<widget class=\"QScrollArea\" name=\"%1\"><attribute "
                          "name=\"title\"><string>%2"
                          "</string></attribute><property "
                          "name=\"widgetResizable\"><bool>true</bool></property><widget "
                          "class=\"QWidget\" "
                          "name=\"%1Contents\"><layout class=\"QVBoxLayout\" name=\"%1Layout\">")
                      .arg(val.first + "Tab" + QString::number(tCnt))
                      .arg(st.first);
            for (const auto &field : st.second) {
                if (field->label.size()) {
                    ts << QString("<item><widget class=\"QLabel\" name=\"label%1\"><property "
                                  "name=\"wordWrap\"><bool>true</bool></property><property "
                                  "name=\"text\"><string>%2</string></property></widget></item>")
                              .arg(++labelCnt)
                              .arg(field->label);
                }
                if (LineField *lf = dynamic_cast<LineField *>(field)) {
                    ts << QString("<item><widget class=\"QLineEdit\" name=\"edit%1\"/></item>")
                              .arg(++editCnt);
                }
                if (TextField *tf = dynamic_cast<TextField *>(field)) {
                    ts << QString("<item><widget class=\"QPlainTextEdit\" name=\"edit%1\">")
                              .arg(++editCnt);
                    if (tf->initialText.size())
                        ts << QString("<property name=\"plainText\"><string>%1</string></property>")
                                  .arg(tf->initialText);
                    ts << "</widget></item>";
                }
            }

            ts << "</layout></widget></widget>";
            ++tCnt;
        }
        ts << "</widget>";
    } else {
        ts << QString(
                  "<widget class=\"QScrollArea\" name=\"%1\"><attribute name=\"title\"><string>%2"
                  "</string></attribute><property "
                  "name=\"widgetResizable\"><bool>true</bool></property><widget class=\"QWidget\" "
                  "name=\"%1Contents\"><layout class=\"QVBoxLayout\" name=\"%1Layout\">")
                  .arg(val.first)
                  .arg(val.second->title());
        for (const auto &field : val.second->_fields) {
            if (field->label.size()) {
                ts << QString("<item><widget class=\"QLabel\" name=\"label%1\"><property "
                              "name=\"wordWrap\"><bool>true</bool></property><property "
                              "name=\"text\"><string>%2</string></property></widget></item>")
                          .arg(++labelCnt)
                          .arg(field->label);
            }
            if (LineField *lf = dynamic_cast<LineField *>(field)) {
                ts << QString("<item><widget class=\"QLineEdit\" name=\"edit%1\"/></item>")
                          .arg(++editCnt);
            }
            if (TextField *tf = dynamic_cast<TextField *>(field)) {
                ts << QString("<item><widget class=\"QPlainTextEdit\" name=\"edit%1\">")
                          .arg(++editCnt);
                if (tf->initialText.size())
                    ts << QString("<property name=\"plainText\"><string>%1</string></property>")
                              .arg(tf->initialText);
                ts << "</widget></item>";
            }
            if (DateField *df = dynamic_cast<DateField *>(field)) {
                ts << QString("<item><widget class=\"QDateEdit\" name=\"edit%1\"/></item>")
                          .arg(++editCnt);
            }
            if (NumberField *df = dynamic_cast<NumberField *>(field)) {
                ts << QString("<item><widget class=\"QSpinBox\" name=\"edit%1\"/></item>")
                          .arg(++editCnt);
            }
        }

        ts << "</layout></widget></widget>";
    }
}
*/
