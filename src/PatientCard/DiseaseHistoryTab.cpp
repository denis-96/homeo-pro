/*
#include "DiseaseHistoryTab.h"
#include <QJsonArray>
#include <QLabel>
#include <QVBoxLayout>

DiseaseHistoryTab::DiseaseHistoryTab(const QString &title, QWidget *parent)
    : PCardTab(title, parent)
{
    auto layout = new QVBoxLayout(this);
    for (const auto &question : questions) {
        auto questionLbl = new QLabel(question, this);
        questionLbl->setWordWrap(true);
        auto answerEdit = new QLineEdit(this);
        auto f = answerEdit->font();
        f.setPointSize(14);
        answerEdit->setFont(f);

        connect(answerEdit, &QLineEdit::editingFinished, this, &PCardTab::changed);

        answerEdits.push_back(answerEdit);

        layout->addWidget(questionLbl);
        layout->addWidget(answerEdit);
        layout->addSpacing(20);
    }

    setLayout(layout);
}

void DiseaseHistoryTab::read(const QJsonValue &json)
{
    if (!json.isArray())
        return;
    QJsonArray answersJson = json.toArray();

    if (answersJson.size() == answerEdits.size())
        for (int i = 0; i < answersJson.size(); ++i) {
            if (answersJson.at(i).isString())
                answerEdits.at(i)->setText(answersJson.at(i).toString());
        }
}

QJsonValue DiseaseHistoryTab::toJson() const
{
    QJsonArray json;

    for (const auto &answer : answerEdits) {
        json.push_back(answer->text());
    }

    return json;
}
*/
