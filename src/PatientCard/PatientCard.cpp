#include "PatientCard.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollArea>
#include <QVBoxLayout>
#include "TableField.h"

PatientCard::PatientCard(QWidget *parent)
    : HomeoEntity{parent}
{
    tabWidget = new QTabWidget(this);

    createTabs();

    auto layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);
}

bool PatientCard::read(QFile &file)
{
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject json = doc.object();

    for (const auto &tab : tabs)
        tab.second->read(json[tab.first]);

    return true;
}

bool PatientCard::write(QFile &file)
{
    QJsonObject json;

    for (const auto &tab : tabs)
        json[tab.first] = tab.second->toJson();

    return file.write(QJsonDocument(json).toJson());
}

void PatientCard::createTabs()
{
    // tabs.push_back({"passportDetails", new PassportDetailsTab("Паспортная часть")});
    tabs.push_back({"complaints",
                    new PCardTab("Жалобы",
                                 {{"",
                                   new TableField({"Основные", "Детализация (ЛОМС)"}),
                                   QVBoxLayout::TopToBottom}})});
    // tabs.push_back({"diseaseHistory", new DiseaseHistoryTab("История заболевния")});
    // tabs.push_back({"lifeHistory", new LifeHistoryTab("История жизни")});

    for (const auto &tab : tabs) {
        auto scrollArea = new QScrollArea(this);
        scrollArea->setWidget(tab.second);
        scrollArea->setWidgetResizable(true);
        tabWidget->addTab(scrollArea, tab.second->title());
        connect(tab.second, &PCardTab::changed, this, &HomeoEntity::changed);
    }
}
