#include "repview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyledItemDelegate>

DrugsView::DrugsView(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent)
{}

void DrugsView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    QHeaderView::paintSection(painter, rect, logicalIndex);
}

class CellsDelegate : public QStyledItemDelegate
{
private:
    QHash<int, QColor> cellColorMapping;

public:
    CellsDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
        cellColorMapping[1] = QColor(0xD7BC3B);
        cellColorMapping[2] = QColor(0x128E48);
        cellColorMapping[3] = QColor(0x2F4C68);
        cellColorMapping[4] = QColor(0x9E3631);
    }

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem customizedOption = option;

        if (index.column() > 0) {
            customizedOption.displayAlignment.setFlag(Qt ::AlignCenter);

            painter->save();

            painter->setRenderHint(QPainter::Antialiasing, true);
            unsigned int degree = index.data(Qt::DisplayRole).toUInt();
            unsigned int importance = index.data(RepModel::Roles::RubricImportance).toUInt();

            if (degree) {
                QPainterPath painterPath;
                QRectF rect(customizedOption.rect);
                rect.setWidth(rect.width() - 2);
                rect.setHeight(rect.height() - 2);
                rect.setLeft(rect.left() + 1);
                rect.setTop(rect.top() + 1);
                painterPath.addRoundedRect(rect, 5, 5);
                painter->fillPath(painterPath, cellColorMapping[degree]);
                painter->setPen(QPen(Qt::white, 1));
                painter->drawPath(painterPath);
            }
            painter->restore();
        }
        QStyledItemDelegate::paint(painter, customizedOption, index);
    }
};

RepView::RepView(QWidget *parent)
    : QTreeView(parent)
{
    setItemDelegate(new CellsDelegate(this));
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setHeader(new DrugsView(this));
    header()->setMinimumSectionSize(25);
    header()->setDefaultSectionSize(25);
    header()->setMinimumHeight(100);
    header()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    header()->setFont(QFont("Segoe UI", 9, QFont::Bold));
    header()->setSectionResizeMode(QHeaderView::Fixed);
}

void RepView::setRepModel(RepModel *repModel)
{
    setModel(repModel);
    setColumnWidth(0, 300);
    header()->setSectionResizeMode(0, QHeaderView::Interactive);
}
