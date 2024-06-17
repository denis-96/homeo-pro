#ifndef PATIENTCARD_H
#define PATIENTCARD_H

#include <QLabel>
#include <QTabWidget>
#include <QWidget>
#include "PCardTab.h"
#include <HomeoEntity.h>

class PatientCard : public HomeoEntity
{
    Q_OBJECT
public:
    explicit PatientCard(QWidget *parent = nullptr);

private:
    bool read(QFile &file) override;
    bool write(QFile &file) override;

    void createTabs();

    QTabWidget *tabWidget;
    std::vector<std::pair<QString, PCardTab *>> tabs;
};

#endif // PATIENTCARD_H
