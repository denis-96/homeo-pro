#ifndef PCARDTAB_H
#define PCARDTAB_H

#include <QBoxLayout>
#include <QWidget>
#include "Field.h"

class PCardTab : public QWidget
{
    Q_OBJECT
public:
    explicit PCardTab(const QString &title = "",
                      const std::vector<Field *> &fields = {},
                      QWidget *parent = nullptr);

    QString title() { return _title; }

    void read(const QJsonValue &json);
    QJsonValue toJson() const;

private:
    QString _title;
    std::vector<Field *> _fields;

signals:
    void changed();
};

#endif // PCARDTAB_H
