#ifndef HOMEOENTITY_H
#define HOMEOENTITY_H

#include <QFile>
#include <QWidget>

class HomeoEntity : public QWidget
{
    Q_OBJECT
public:
    explicit HomeoEntity(QWidget *parent = nullptr);

    QString fileName();
    bool save(const QString &fileName = "");
    bool load(const QString &fileName);

private:
    QString _fileName;

    virtual bool read(QFile &file) = 0;
    virtual bool write(QFile &file) = 0;

signals:
    void changed();
    void saved(const QString &fileName);
};

inline QString HomeoEntity::fileName()
{
    return _fileName;
}

#endif // HOMEOENTITY_H
