#include "repview.h"

#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMouseEvent>
#include <QPainter>
#include <QStyledItemDelegate>

class RepSelectionModel : public QItemSelectionModel
{
public:
    RepSelectionModel(QAbstractItemModel *model, QObject *parent = nullptr)
        : QItemSelectionModel(model, parent)
    {}
    void select(const QItemSelection &selection,
                QItemSelectionModel::SelectionFlags command) override
    {
        if (command & QItemSelectionModel::Select) {
            QItemSelection newSelection;
            for (const auto &range : selection)
                if (range.top() == range.bottom())
                    newSelection.select(model()->index(range.top(), 0, range.parent()),
                                        model()->index(range.bottom(),
                                                       model()->columnCount() - 1,
                                                       range.parent()));

            for (const auto &range : newSelection)
                for (const auto &i : range.indexes())
                    if (i.data().toUInt())
                        newSelection.select(model()->index(0, i.column()),
                                            model()->index(model()->rowCount() - 1, i.column()));
            QItemSelectionModel::select(newSelection, command & ~QItemSelectionModel::Rows);
        } else
            QItemSelectionModel::select(selection, command);
    }
};
class DrugsView : public QHeaderView
{
public:
    DrugsView(QWidget *parent = nullptr)
        : QHeaderView(Qt::Horizontal, parent)
    {
        auto pal = palette();
        pal.setColor(QPalette::Base, QColor(0xD2D2D2));
        setPalette(pal);
    }

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override
    {
        if (!logicalIndex && model()->columnCount() > 1) {
            painter->save();
            painter->setPen(Qt::black);
            painter->setBrush(Qt::NoBrush);
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawLine(rect.bottomLeft(), rect.bottomRight());
            painter->drawLine(rect.topRight(), rect.bottomRight());
            painter->restore();
            return;
        }
        QString text = model()->headerData(logicalIndex, Qt::Horizontal).toString();

        painter->save();
        painter->translate(rect.left() + 12, rect.bottom());
        painter->rotate(-70);

        RepView *repView = qobject_cast<RepView *>(parentWidget());
        if (repView && repView->selectionModel()) {
            if (repView->selectionModel()->isColumnSelected(logicalIndex)) {
                auto font_metrics = painter->fontMetrics();
                auto text_rect = font_metrics.boundingRect(text);
                text_rect.setTop(text_rect.top() + 5);
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(0xAC41FF));
                painter->setRenderHint(QPainter::Antialiasing);
                painter->drawRect(text_rect);
            }
        }

        painter->setPen(Qt::black);
        painter->drawText(0, 0, text);
        painter->restore();
    }
};

class CellsDelegate : public QStyledItemDelegate
{
private:
    QHash<int, QColor> cellColorMapping{{1, QColor(0xFFF738)},
                                        {2, QColor(0x00FF75)},
                                        {3, QColor(0x4D9FFF)},
                                        {4, QColor(0xFF4949)}};

public:
    CellsDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {}

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        unsigned importance = index.data(RepModel::RubricImportance).toUInt();
        if (index.column()) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(Qt::black);
            painter->setBrush(Qt::white);
            if ((opt.state & QStyle::State_Selected)
                || qobject_cast<RepView *>(parent())->selectionModel()->isColumnSelected(
                    index.column())) {
                painter->setBrush(QColor(0xAC41FF));
            }
            painter->drawRect(opt.rect);
            painter->restore();

            painter->save();
            if (unsigned backgroundColorId = index.data(Qt::DisplayRole).toUInt()) {
                QColor backgroundColor = importance ? cellColorMapping[backgroundColorId]
                                                    : QColor(Qt::lightGray);
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setBrush(backgroundColor);
                painter->setPen(Qt::NoPen);
                QRect r = opt.rect;
                r.setTop(r.top() + 1);
                r.setBottom(r.bottom() - 1);
                r.setLeft(r.left() + 1);
                r.setRight(r.right() - 1);
                painter->drawRoundedRect(r, 15, 15);
            }
            painter->setPen(Qt::black);
            painter->drawText(opt.rect, Qt::AlignCenter, index.data().toString());
            painter->restore();
        } else {
            painter->save();
            if (opt.state & QStyle::State_Selected) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(0xAC41FF));
                painter->drawRect(opt.rect);
            }
            painter->setPen(Qt::black);
            if (importance == 0)
                painter->setPen(QColor(0x595959));
            if (importance > 1) {
                auto font = painter->font();
                font.setBold(true);
                painter->setFont(font);
            }
            painter->drawText(opt.rect, Qt::AlignVCenter | Qt::AlignLeft, index.data().toString());
            painter->restore();
        }
    }
};

RepView::RepView(QWidget *parent)
    : QTreeView(parent)
{
    setItemDelegate(new CellsDelegate(this));

    setHeader(new DrugsView(this));
    header()->setMinimumSectionSize(25);
    header()->setDefaultSectionSize(25);
    header()->setMinimumHeight(100);
    header()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    header()->setFont(QFont("Segoe UI", 12, QFont::Bold));
    header()->setSectionResizeMode(QHeaderView::Fixed);
    auto pal = palette();
    pal.setColor(QPalette::Base, QColor(0xD2D2D2));
    setPalette(pal);
}

void RepView::setRepModel(RepModel *repModel)
{
    QTreeView::setModel(repModel);
    setSelectionModel(new RepSelectionModel(repModel, this));
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setColumnWidth(0, 300);
    header()->setSectionResizeMode(0, QHeaderView::Interactive);
}
