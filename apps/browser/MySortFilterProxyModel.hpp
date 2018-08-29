//
// Created by koncord on 30.01.17.
//

#pragma once

#include <QSortFilterProxyModel>

class MySortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_FINAL;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const Q_DECL_FINAL;
public:
    explicit MySortFilterProxyModel(QObject *parent);
    void filterFullServer(bool state);
    void filterEmptyServers(bool state);
    void filterPassworded(bool state);
    void pingLessThan(int maxPing);
private:
    bool filterEmpty, filterFull, filterPasswEnabled;
    int maxPing;
};
