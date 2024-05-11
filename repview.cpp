#include "repview.h"
#include <QHeaderView>
#include <QPainter>
#include <QStyledItemDelegate>

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
            unsigned int itemData = index.data(Qt::DisplayRole).toUInt();

            auto backgroundColor = cellColorMapping[itemData];

            painter->fillRect(option.rect, backgroundColor);

            // customizedOption.displayAlignment.setFlag(Qt ::AlignCenter);
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

    // verticalHeader()->setMinimumSectionSize(20);
    // verticalHeader()->setDefaultSectionSize(20);
    // verticalHeader()->hide();

    // horizontalHeader()->setMinimumSectionSize(20);
    // horizontalHeader()->setDefaultSectionSize(20);

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
