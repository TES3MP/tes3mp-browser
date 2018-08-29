//
// Created by koncord on 27.05.17.
//

#pragma once

#include <QObject>
#include <vector>
#include <QAbstractItemModel>
#include "Data.hpp"

class QueryHelper : public QObject
{
Q_OBJECT
public:
    explicit QueryHelper(QAbstractItemModel *model);
public slots:
    void refresh();
    void terminate();
private slots:
    void update(const Server& data);
signals:
    void finished();
    void started();
private:
    QAbstractItemModel *_model;
};

