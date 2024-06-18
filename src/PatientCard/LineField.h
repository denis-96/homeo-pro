#ifndef LINEFIELD_H
#define LINEFIELD_H

#include <QBoxLayout>
#include <QLineEdit>
#include "Field.h"

class LineField : public Field
{
    Q_OBJECT
public:
    LineField(const QString &label,
              QBoxLayout::Direction layout = QBoxLayout::TopToBottom,
              QWidget *parent = nullptr);

    void read(const QJsonValue &json) override;
    QJsonValue toJson() const override;

private:
    QLineEdit *lineEdit;
};

#endif // LINEFIELD_H
