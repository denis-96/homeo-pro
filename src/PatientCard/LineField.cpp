#include "LineField.h"
#include <QLabel>

LineField::LineField(const QString &label, QBoxLayout::Direction layout, QWidget *parent)
    : Field(parent)
{
    lineEdit = new QLineEdit(this);
    auto f = lineEdit->font();
    f.setPointSize(14);
    lineEdit->setFont(f);
    connect(lineEdit, &QLineEdit::editingFinished, this, &Field ::changed);

    auto lbl = new QLabel(label, this);
    lbl->setWordWrap(true);

    setLayout(new QBoxLayout(layout, this));
    this->layout()->addWidget(lbl);
    this->layout()->addWidget(lineEdit);
}

void LineField::read(const QJsonValue &json)
{
    if (json.isString())
        lineEdit->setText(json.toString());
}

QJsonValue LineField::toJson() const
{
    return lineEdit->text();
}
