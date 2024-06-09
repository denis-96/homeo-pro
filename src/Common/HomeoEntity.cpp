#include "HomeoEntity.h"

HomeoEntity::HomeoEntity(QWidget *parent)
    : QWidget{parent}
{}

bool HomeoEntity::load(const QString &fileName)
{
    if (fileName.size()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QFile::Text) && read(file)) {
            _fileName = fileName;
            return true;
        }
    }
    return false;
}

bool HomeoEntity::save(const QString &fileName)
{
    QString fName = _fileName;

    if (fileName.size())
        fName = fileName;

    if (fName.isEmpty())
        return false;

    QFile file(fName);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    if (!write(file))
        return false;

    _fileName = fName;
    emit saved(_fileName);
    return true;
}
