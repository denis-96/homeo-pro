#ifndef PCARDTAB_H
#define PCARDTAB_H

#include <QBoxLayout>
#include <QWidget>
#include "PCardField.h"

class PCardTab : public QWidget
{
    Q_OBJECT
public:
    struct LabeledField
    {
        QString label;
        PCardField *field;
        QBoxLayout::Direction alignment;
    };

    explicit PCardTab(const QString &title = "",
                      const std::vector<LabeledField> &fields = {},
                      QWidget *parent = nullptr);

    QString title() { return _title; }

    void read(const QJsonValue &json);
    QJsonValue toJson() const;

private:
    QString _title;
    std::vector<PCardField *> _fields;

signals:
    void changed();
};

#endif // PCARDTAB_H
