#include "DateField.h"
#include <QLabel>

DateField::DateField(const QString &label, QBoxLayout::Direction layout, QWidget *parent)
    : Field(parent)
{
    dateEdit = new QDateEdit(this);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setMinimumDate(QDate(2000, 1, 1));
    dateEdit->setMaximumDate(QDate::currentDate());
    auto f = dateEdit->font();
    f.setPointSize(14);
    dateEdit->setFont(f);

    connect(dateEdit, &QDateEdit::editingFinished, this, &Field ::changed);

    auto lbl = new QLabel(label, this);
    lbl->setWordWrap(true);

    setLayout(new QBoxLayout(layout, this));
    this->layout()->addWidget(lbl);
    this->layout()->addWidget(dateEdit);
}

void DateField::read(const QJsonValue &json)
{
    if (json.isDouble())
        dateEdit->setDate(QDate::fromJulianDay(json.toInteger()));
}

QJsonValue DateField::toJson() const
{
    return dateEdit->date().toJulianDay();
}
