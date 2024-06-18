#ifndef NUMBERFIELD_H
#define NUMBERFIELD_H

#include <QBoxLayout>
#include <QSpinBox>
#include "Field.h"

class NumberField : public Field
{
    Q_OBJECT
public:
    NumberField(const QString &label,
                QBoxLayout::Direction layout = QBoxLayout::TopToBottom,
                QWidget *parent = nullptr);

    void read(const QJsonValue &json) override;
    QJsonValue toJson() const override;

private:
    QSpinBox *numberEdit;
};

#endif // NUMBERFIELD_H
