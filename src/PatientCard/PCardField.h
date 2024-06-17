#ifndef PCARDFIELD_H
#define PCARDFIELD_H

#include <QBoxLayout>
#include <QJsonValue>
#include <QWidget>

class PCardField : QWidget
{
    Q_OBJECT
public:
    PCardField(const QString &label = "",
               QWidget *parent = nullptr,
               QBoxLayout::Direction layout = QBoxLayout::TopToBottom);

    virtual void read(const QJsonValue &json) = 0;
    virtual QJsonValue toJson() const = 0;

signals:
    void changed();
};

#endif // PCARDFIELD_H
