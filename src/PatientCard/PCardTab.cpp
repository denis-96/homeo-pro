#include "PCardTab.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QLabel>
#include <QVBoxLayout>

PCardTab::PCardTab(const QString &title, const std::vector<Field *> &fields, QWidget *parent)
    : _title(title)
    , _fields(fields)
    , QWidget{parent}
{
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);

    for (const auto &field : _fields) {
        layout()->addWidget(field);
        connect(field, &Field::changed, this, &PCardTab::changed);
    }
}

void PCardTab::addField(Field *field)
{
    _fields.push_back(field);
    connect(field, &Field::changed, this, &PCardTab::changed);
}

void PCardTab::read(const QJsonValue &json)
{
    if (!json.isArray())
        return;
    QJsonArray answersJson = json.toArray();

    if (answersJson.size() == _fields.size())
        for (int i = 0; i < answersJson.size(); ++i) {
            _fields.at(i)->read(answersJson.at(i));
        }
}

QJsonValue PCardTab::toJson() const
{
    QJsonArray json;

    for (const auto &field : _fields) {
        json.push_back(field->toJson());
    }

    return json;
}
