#ifndef PATIENTCARD_H
#define PATIENTCARD_H

#include <QLabel>
#include <QTabWidget>
#include <QWidget>
#include <HomeoEntity.h>

class PatientCard : public HomeoEntity
{
    Q_OBJECT
public:
    explicit PatientCard(QWidget *parent = nullptr);

private:
    bool read(QFile &file) override;
    bool write(QFile &file) override;

    QTabWidget *tabWidget;

private slots:
};

#endif // PATIENTCARD_H
