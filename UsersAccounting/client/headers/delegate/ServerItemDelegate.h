//
// Created by saveliy on 25.11.2025.
//

#pragma once
#include <QStyledItemDelegate>


class ServerItemDelegate: public QStyledItemDelegate {
public:
    explicit ServerItemDelegate(QObject* parent): QStyledItemDelegate(parent){};

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
