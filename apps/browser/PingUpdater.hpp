//
// Created by koncord on 02.05.17.
//

#pragma once

#include <QObject>
#include <QVector>
#include <QtCore/QThread>

#include "Types.hpp"

class PingUpdater : public QObject
{
    Q_OBJECT
public:
    PingUpdater(QObject *parent = 0): QObject(parent) {}
    void addServer(int row, const AddrPair &addrPair);
public slots:
    void stop();
    void process();
signals:
    void start(QThread::Priority priority = QThread::InheritPriority);
    void updateModel(int row, unsigned ping);
    void finished();
private:
    QVector<ServerRow> servers;
    bool run;
};
