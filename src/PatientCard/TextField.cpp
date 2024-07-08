#include "TextField.h"
#include <QLabel>
#include <QVBoxLayout>

TextField::TextField(const QString &label, const QString &initialText, QWidget *parent)
    : Field(parent)
{
    textEdit = new QPlainTextEdit(this);
    auto f = textEdit->font();
    f.setPointSize(14);
    textEdit->setFont(f);

    if (initialText.size())
        textEdit->setPlainText(initialText);

    connect(textEdit, &QPlainTextEdit::textChanged, this, &Field ::changed);

    auto lbl = new QLabel(label, this);
    lbl->setWordWrap(true);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(lbl);
    layout()->addWidget(textEdit);
}

void TextField::read(const QJsonValue &json)
{
    if (json.isString())
        textEdit->setPlainText(json.toString());
}

QJsonValue TextField::toJson() const
{
    return {textEdit->toPlainText()};
}
