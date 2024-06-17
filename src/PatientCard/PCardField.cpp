#include "PCardField.h"
#include <QLabel>

PCardField::PCardField(const QString &label, QWidget *parent, QBoxLayout::Direction layout)
    : QWidget(parent)
{
    setLayout(new QBoxLayout(layout, this));
    this->layout()->addWidget(new QLabel(label, this));
}
