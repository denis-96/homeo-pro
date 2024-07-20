#ifndef PATIENTCARD_H
#define PATIENTCARD_H

#include <QTabWidget>
#include <QTableWidget>
#include <QWidget>
#include <HomeoEntity.h>

namespace Ui {
class PatientCard;
}
class PatientCard : public HomeoEntity
{
    Q_OBJECT
public:
    explicit PatientCard(QWidget *parent = nullptr);
    ~PatientCard();

private:
    bool read(QFile &file) override;
    bool write(QFile &file) override;

    void setupFields();

    void setupEditableTable(QTableWidget *table);

    Ui::PatientCard *ui;
    QTabWidget *tabWidget;

    std::vector<QWidget *> edits;
    std::vector<QTableWidget *> tables;
};

#endif // PATIENTCARD_H
