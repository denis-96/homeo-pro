#include "NumberField.h"
#include <QLabel>

NumberField::NumberField(const QString &label, QBoxLayout::Direction layout, QWidget *parent)
{
    numberEdit = new QSpinBox(this);
    auto f = numberEdit->font();
    f.setPointSize(14);
    numberEdit->setFont(f);
    connect(numberEdit, &QSpinBox::editingFinished, this, &Field ::changed);

    auto lbl = new QLabel(label, this);
    lbl->setWordWrap(true);

    setLayout(new QBoxLayout(layout, this));
    this->layout()->addWidget(lbl);
    this->layout()->addWidget(numberEdit);
}

void NumberField::read(const QJsonValue &json)
{
    if (json.isDouble())
        numberEdit->setValue(json.toInt());
}

QJsonValue NumberField::toJson() const
{
    return numberEdit->value();
}
