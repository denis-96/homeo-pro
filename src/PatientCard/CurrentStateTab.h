#ifndef CURRENTSTATETAB_H
#define CURRENTSTATETAB_H

#include "PCardTab.h"

class CurrentStateTab : public PCardTab
{
    Q_OBJECT
public:
    CurrentStateTab(const QString &title = "",
                    const std::vector<std::pair<QString, std::vector<Field *>>> &content = {},
                    QWidget *parent = nullptr);
};

#endif // CURRENTSTATETAB_H
