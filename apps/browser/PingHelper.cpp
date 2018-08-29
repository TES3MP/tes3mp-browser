//
// Created by koncord on 03.05.17.
//

#include "PingHelper.hpp"
#include "ServerModel.hpp"
#include <QDebug>
#include "PingUpdater.hpp"

void PingHelper::Add(int row, const AddrPair &addrPair)
{
    pingUpdater->addServer(row, addrPair);
    if (!pingThread->isRunning())
        pingThread->start();
}

void PingHelper::Reset()
{
    //if (pingThread->isRunning())
    Stop();
}

void PingHelper::Stop()
{
    emit pingUpdater->stop();
}

void PingHelper::SetModel(QAbstractTableModel *model)
{
    this->model = model;
}

void PingHelper::update(int row, unsigned ping)
{
    model->setData(model->index(row, Server::PING), ping);
}

PingHelper &PingHelper::Get()
{
    static PingHelper helper;
    return helper;
}

PingHelper::PingHelper() : QObject()
{
    pingThread = new QThread;
    pingUpdater = new PingUpdater;
    pingUpdater->moveToThread(pingThread);

    connect(pingThread, &QThread::started, pingUpdater, &PingUpdater::process);
    connect(pingUpdater, &PingUpdater::start, pingThread, &QThread::start);
    connect(pingUpdater, &PingUpdater::finished, pingThread, &QThread::quit);
    connect(this, &PingHelper::stop, pingUpdater, &PingUpdater::stop);
    //connect(pingUpdater, SIGNAL(finished()), pingUpdater, SLOT(deleteLater()));
    connect(pingUpdater, &PingUpdater::updateModel, this, &PingHelper::update);
}
