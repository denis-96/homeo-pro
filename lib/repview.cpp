#include "repview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyledItemDelegate>

DrugsView::DrugsView(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent)
{}

void DrugsView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (!rect.isValid())
        return;
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->translate(rect.bottomLeft());
    painter->rotate(-90);
    QRect newRect = QRect(0, -1, rect.height(), rect.width());

    painter->save();
    QHeaderView::paintSection(painter, newRect, logicalIndex);
    painter->restore();

    painter->setBrush(QBrush(Qt::NoBrush));
    painter->setPen(Qt::gray);
    painter->drawRect(newRect);
    painter->restore();
}

class CellsDelegate : public QStyledItemDelegate
{
private:
    QHash<int, Qt::GlobalColor> cellColorMapping;

public:
    CellsDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
        cellColorMapping[0] = Qt::white;
        cellColorMapping[1] = Qt::yellow;
        cellColorMapping[2] = Qt::green;
        cellColorMapping[3] = Qt::blue;
        cellColorMapping[4] = Qt::red;
    }

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem customizedOption = option;

        if (index.column() > 0) {
            unsigned int degree = index.data(Qt::DisplayRole).toUInt();
            unsigned int importance = index.data(RepModel::Roles::RubricImportance).toUInt();
            auto backgroundColor = !importance ? Qt::white : cellColorMapping[degree];

            painter->fillRect(option.rect, backgroundColor);

            customizedOption.displayAlignment.setFlag(Qt ::AlignCenter);
        }

        QStyledItemDelegate::paint(painter, customizedOption, index);
    }
};

RepView::RepView(QWidget *parent)
    : QTreeView(parent)
{
    setItemDelegate(new CellsDelegate(this));
    // setFont(QFont("Segoe UI", 13));
    // setStyleSheet("QHeaderView::section { background-color: palette(midlight) }");
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setHeader(new DrugsView(this));
    header()->setMinimumSectionSize(20);
    header()->setDefaultSectionSize(20);
    header()->setFixedHeight(100);

    // horizontalHeader()->setFont(QFont("Segoe UI", 10, QFont::Bold));
    // horizontalHeader()->setSectionsClickable(false);
    // horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignBottom
    //                                         | (Qt::Alignment) Qt::TextWordWrap);
}

void RepView::setRepModel(RepModel *repModel)
{
    setModel(repModel);
    /*
    setColumnWidth(0, 400);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    horizontalHeader()->setMinimumHeight(150);
*/
}
