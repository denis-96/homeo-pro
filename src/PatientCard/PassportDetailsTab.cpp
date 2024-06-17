/*
#include "PassportDetailsTab.h"
#include <QFormLayout>
#include <QJsonObject>

PassportDetailsTab::PassportDetailsTab(const QString &title, QWidget *parent)
    : PCardTab(title, parent)
{
    nameEdit = new QLineEdit(this);
    connect(nameEdit, &QLineEdit::editingFinished, this, &PCardTab::changed);

    ageEdit = new QSpinBox(this);
    connect(ageEdit, &QSpinBox::editingFinished, this, &PCardTab::changed);

    occupationEdit = new QLineEdit(this);
    connect(occupationEdit, &QLineEdit::editingFinished, this, &PCardTab::changed);

    appealDateEdit = new QDateEdit(this);
    appealDateEdit->setDate(QDate::currentDate());
    appealDateEdit->setMinimumDate(QDate(2000, 1, 1));
    appealDateEdit->setMaximumDate(QDate::currentDate());
    connect(appealDateEdit, &QDateEdit::editingFinished, this, &PCardTab::changed);

    auto layout = new QFormLayout(this);
    layout->addRow("Фамилия Имя Отчество", nameEdit);
    layout->addRow("Возраст", ageEdit);
    layout->addRow("Профессия, место работы (учебы), должность", occupationEdit);
    layout->addRow("Дата обращения", appealDateEdit);
    setLayout(layout);

    auto f = font();
    f.setPointSize(14);
    setFont(f);
}

void PassportDetailsTab::read(const QJsonValue &json)
{
    if (!json.isObject())
        return;
    QJsonObject detailsJson = json.toObject();

    if (const QJsonValue v = detailsJson["name"]; v.isString())
        nameEdit->setText(v.toString());

    if (const QJsonValue v = detailsJson["age"]; v.isDouble())
        ageEdit->setValue(v.toInt());

    if (const QJsonValue v = detailsJson["occupation"]; v.isString())
        occupationEdit->setText(v.toString());

    if (const QJsonValue v = detailsJson["appealDate"]; v.isDouble())
        appealDateEdit->setDate(QDate::fromJulianDay(v.toInteger()));
}

QJsonValue PassportDetailsTab::toJson() const
{
    QJsonObject json;

    json["name"] = nameEdit->text();
    json["age"] = ageEdit->value();
    json["occupation"] = occupationEdit->text();
    json["appealDate"] = appealDateEdit->date().toJulianDay();

    return json;
}
*/
