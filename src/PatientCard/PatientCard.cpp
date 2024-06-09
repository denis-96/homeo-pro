#include "PatientCard.h"
#include <QVBoxLayout>
#include "ComplaintsTab.h"
#include "PassportDetailsTab.h"

PatientCard::PatientCard(QWidget *parent)
    : HomeoEntity{parent}
{
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(new PassportDetailsTab(), "Паспортная часть");
    tabWidget->addTab(new ComplaintsTab(), "Жалобы");

    auto layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

bool PatientCard::read(QFile &file)
{
    return true;
}

bool PatientCard::write(QFile &file)
{
    return true;
}
