#include "PassportDetailsTab.h"
#include <QVBoxLayout>

PassportDetailsTab::PassportDetailsTab(QWidget *parent)
    : QWidget{parent}
{
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Фамилия Имя Отчество");

    ageEdit = new QSpinBox(this);

    occupationEdit = new QLineEdit(this);
    occupationEdit->setPlaceholderText("Профессия, место работы (учебы), должность");

    appealDateEdit = new QDateEdit(QDate(24, 1, 1), this);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(nameEdit);
    layout->addWidget(ageEdit);
    layout->addWidget(occupationEdit);
    layout->addWidget(appealDateEdit);
    setLayout(layout);

    auto f = font();
    f.setPointSize(14);
    setFont(f);
}
