#ifndef PASSPORTDETAILSTAB_H
#define PASSPORTDETAILSTAB_H

#include <QDateEdit>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>

class PassportDetailsTab : public QWidget
{
    Q_OBJECT
public:
    explicit PassportDetailsTab(QWidget *parent = nullptr);

private:
    QLineEdit *nameEdit;
    QSpinBox *ageEdit;
    QLineEdit *occupationEdit;
    QDateEdit *appealDateEdit;
};

#endif // PASSPORTDETAILSTAB_H
