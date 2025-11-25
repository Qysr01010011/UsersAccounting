//
// Created by saveliy on 25.11.2025.
//

#include "delegate/ServerItemDelegate.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QApplication>


void ServerItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem o = option;
    o.text.clear();
    const QWidget* widget = option.widget;
    QStyle* style = widget != nullptr ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &o, painter, widget);


    QString title = index.data(Qt::DisplayRole).toString();
    QString ip = index.data(Qt::UserRole).toString();
    QString port = index.data(Qt::UserRole + 1).toString();
    QString subtitle = ip + ":" + port;

    QRect r = option.rect;

    painter->save();
    painter->setPen(option.palette.color(QPalette::Text));
    painter->drawText(QRect(r.left(), r.top(), r.width(), r.height()/2), Qt::AlignLeft | Qt::AlignVCenter, title);
    painter->setPen(option.palette.color(QPalette::PlaceholderText));
    painter->drawText(QRect(r.left(), r.top() + (r.height()/2), r.width(), r.height()/2), Qt::AlignLeft | Qt::AlignVCenter, subtitle);
    painter->restore();
}


QSize ServerItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(size.height() * 2);
    return size;
}
