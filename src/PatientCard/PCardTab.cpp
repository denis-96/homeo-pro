#include "PCardTab.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QLabel>
#include <QVBoxLayout>

PCardTab::PCardTab(const QString &title, const std::vector<LabeledField> &fields, QWidget *parent)
    : _title(title)
    , QWidget{parent}
{
    auto layout = new QVBoxLayout(this);

    _fields.reserve(fields.size());
    for (const auto &field : fields) {
        _fields.push_back(field.field);

        auto label = new QLabel(field.label);
        label->setWordWrap(true);

        auto fieldLayout = new QBoxLayout(field.alignment, this);
        fieldLayout->addWidget(label);
        fieldLayout->addWidget(dynamic_cast<QWidget *>(field.field));

        layout->addLayout(fieldLayout);
    }

    setLayout(layout);
}

void PCardTab::read(const QJsonValue &json)
{
    if (!json.isArray())
        return;
    QJsonArray answersJson = json.toArray();

    if (answersJson.size() == _fields.size())
        for (int i = 0; i < answersJson.size(); ++i) {
            if (answersJson.at(i).isString())
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
