#include "CurrentStateTab.h"
#include <QScrollArea>
#include <QTabWidget>

CurrentStateTab::CurrentStateTab(
    const QString &title,
    const std::vector<std::pair<QString, std::vector<Field *> > > &content,
    QWidget *parent)
    : PCardTab(title, {}, parent)
{
    auto tabWidget = new QTabWidget(this);
    tabWidget->setTabPosition(QTabWidget ::West);

    for (const auto &tab : content) {
        QWidget *widget = new QWidget(this);
        widget->setLayout(new QVBoxLayout(widget));
        widget->layout()->setContentsMargins(0, 0, 0, 0);

        for (const auto &field : tab.second) {
            widget->layout()->addWidget(field);
            addField(field);
        }

        auto scrollArea = new QScrollArea(this);
        scrollArea->setWidget(widget);
        scrollArea->setWidgetResizable(true);
        tabWidget->addTab(scrollArea, tab.first);
    }

    layout()->addWidget(tabWidget);
}
