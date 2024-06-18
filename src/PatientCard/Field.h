#ifndef FIELD_H
#define FIELD_H

#include <QJsonValue>
#include <QWidget>

class Field : public QWidget
{
    Q_OBJECT
public:
    Field(QWidget *parent = nullptr);

    virtual void read(const QJsonValue &json) = 0;
    virtual QJsonValue toJson() const = 0;

signals:
    void changed();
};

#endif // FIELD_H
