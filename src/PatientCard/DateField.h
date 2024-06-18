#ifndef DATEFIELD_H
#define DATEFIELD_H

#include <QBoxLayout>
#include <QDateEdit>
#include "Field.h"

class DateField : public Field
{
    Q_OBJECT
public:
    DateField(const QString &label,
              QBoxLayout::Direction layout = QBoxLayout::TopToBottom,
              QWidget *parent = nullptr);

    void read(const QJsonValue &json) override;
    QJsonValue toJson() const override;

private:
    QDateEdit *dateEdit;
};

#endif // DATEFIELD_H
