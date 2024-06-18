#include "PatientCard.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollArea>
#include <QVBoxLayout>
#include "DateField.h"
#include "LineField.h"
#include "NumberField.h"
#include "TableField.h"
#include "TextField.h"

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
    tabs.push_back({"passportDetails",
                    new PCardTab("Паспортная часть",
                                 {new LineField("Фамилия Имя Отчество", QBoxLayout::LeftToRight),
                                  new NumberField("Возраст", QBoxLayout::LeftToRight),
                                  new LineField("Профессия, место работы (учебы), должность",
                                                QBoxLayout::LeftToRight),
                                  new DateField("Дата обращения", QBoxLayout::LeftToRight)})});
    tabs.push_back(
        {"complaints",
         new PCardTab("Жалобы",
                      {new TableField("", {"Основные", "Детализация (ЛОМС)"}, [](int section) {
                          return QString::number(section + 1);
                      })})});

    tabs.push_back(
        {"diseaseHistory",
         new PCardTab(
             "История заболевния",
             {
                 new TextField("В течение какого времени считает себя больным?"),
                 new TextField("Факторы, способствовавшие началу заболевания."),
                 new TextField("С каких признаков началось заболевание?"),
                 new TextField("Первое обращение к врачу, результаты проводившихся исследований, "
                               "диагноз заболевания, лечение в тот период, его эффективность."),
                 new TextField(
                     "Последующее течение заболевания:\nа) динамика начальных симптомов, появление "
                     "новых симптомов и дальнейшее развитие всех признаков заболевания;\nб) "
                     "частота обострений, длительность ремиссий, осложнения заболевания;\nв) "
                     "применявшиеся лечебные и диагностические мероприятия"),
             })});

    tabs.push_back(
        {"lifeHistory",
         new PCardTab(
             "История жизни",
             {
                 new TextField("Краткие биографические данные:\nГод и место рождения, в какой "
                               "семье родился, каким ребенком по счету, как рос и развивался."),
                 new TextField(
                     "Образование: неполное или полное среднее, специальное среднее или высшее."),
                 new TextField("Отношение к военной службе, пребывание на фронте (если был "
                               "освобожден или демобилизован, то указать, по какой причине)."),
                 new TextField(
                     "Семейно-половой анамнез:\nДля женщин - время появления менструаций, "
                     "длительность менструального цикла, продолжительность менструаций, количество "
                     "отделений. Возраст вступления в брак. Беременности, роды и аборты "
                     "(самопроизвольные и искусственные), их количество и осложнения."),
                 new TextField("Климакс и его течение. Время окончания менструаций (менопауза). "
                               "Семейное положение в настоящий момент. Количество детей."),
                 new TextField(
                     "Трудовой анамнез:\nС какого возраста, кем и где начал работать. Дальнейшая "
                     "трудовая деятельность в хронологической последовательности с указанием мест "
                     "работы и должностей. Условия и режим труда. Профессиональные вредности "
                     "(воздействие токсических химических соединений, пылей, ионизирующей "
                     "радиации, физические и эмоциональные нагрузки и др. вредности)."),
                 new TextField("Бытовой анамнез:\nЖилищные и санитарно-гигиенические условия в "
                               "быту (площадь помещений, их характеристика, наличие удобств). "
                               "Количество проживающих на данной жилплощади лиц. Климатические "
                               "условия. Пребывание в зонах экологических бедствий."),
                 new TextField("Питание:\nРежим и регулярность питания, характер пищи, ее "
                               "разнообразие, калорийность и т.д."),
                 new TextField("Вредные привычки:\nКурение, с какого возраста, в каком количестве. "
                               "Употребление алкоголя, с какого возраста, в каком количестве. "
                               "Употребление наркотиков."),
                 new TableField(
                     "Перенесенные заболевания:\nВ хронологической последовательности излагаются "
                     "все перенесенные заболевания, операции, ранения, травмы, контузии с "
                     "указанием возраста больного, длительности и тяжести этих заболеваний и их "
                     "осложнений и проводившегося лечения. Особо указать на перенесенные "
                     "инфекционные заболевания, венерические болезни, туберкулез, желтуху. "
                     "Отметить, переливалась ли ранее кровь или кровезаменители, и были ли реакции "
                     "на переливание. Указать, проводилось ли парентеральное введение лекарств в "
                     "течение последних 6 месяцев.",
                     {"Этиология", "Перенесенные заболевания"},
                     [](int section) -> QString {
                         if (section == 0)
                             return "В у";
                         if (section == 1)
                             return "Роды";
                         if (section == 2)
                             return "До 1 года";
                         if (section == 3)
                             return "1 - 4";

                         return QString("%1 - %2")
                             .arg((section - 3) * 4)
                             .arg((section - 3) * 4 + (section < 9 ? 4 : 12));
                     },
                     10),
                 new TextField("Аллергологический анамнез:\nНепереносимость пищевых продуктов, "
                               "различных медикаментов, вакцин и сывороток. Наличие аллергических "
                               "реакций (вазомоторного ринита, крапивницы, отека Квинке), причины "
                               "этих реакций, их сезонность."),
             })});
    tabs.push_back({"currentState", new PCardTab("Настоящее состояние")});

    // tabWidget->setTabPosition(QTabWidget ::West);
    for (const auto &tab : tabs) {
        auto scrollArea = new QScrollArea(this);
        scrollArea->setWidget(tab.second);
        scrollArea->setWidgetResizable(true);
        tabWidget->addTab(scrollArea, tab.second->title());
        connect(tab.second, &PCardTab::changed, this, &HomeoEntity::changed);
    }
}
