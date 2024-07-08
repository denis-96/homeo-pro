#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include "Field.h"
#include "QPlainTextEdit"

class TextField : public Field
{
    Q_OBJECT
public:
    TextField(const QString &label, const QString &initialText = "", QWidget *parent = nullptr);

    void read(const QJsonValue &json) override;
    QJsonValue toJson() const override;

private:
    QPlainTextEdit *textEdit;
};

#endif // TEXTFIELD_H
