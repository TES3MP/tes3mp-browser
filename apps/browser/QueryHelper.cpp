//
// Created by koncord on 27.05.17.
//

#include "netutils/MasterClient.hpp"
#include "netutils/Utils.hpp"
#include "ServerModel.hpp"
#include "QueryHelper.hpp"
#include "PingHelper.hpp"

QueryHelper::QueryHelper(QAbstractItemModel *model)
{
    _model = model;

    connect(MasterClient::get(), &MasterClient::server, this, &QueryHelper::update);
    connect(MasterClient::get(), &MasterClient::finished, this, &QueryHelper::finished);
}

void QueryHelper::refresh()
{

    //if (!queryThread->isRunning())
    {
        _model->removeRows(0, _model->rowCount());
        PingHelper::Get().Stop();
        MasterClient::get()->requestList();
        //queryThread->start();
        emit started();
    }
}

void QueryHelper::terminate()
{

}

void QueryHelper::update(const Server& data)
{
    ServerModel *model = ((ServerModel*)_model);
    model->insertRow(model->rowCount());
    int row = model->rowCount() - 1;

    QModelIndex mi = model->index(row, Server::ADDR);
    model->setData(mi, QString::fromStdString(data.address));

    mi = model->index(row, Server::PORT);
    model->setData(mi, data.port);

    mi = model->index(row, Server::ID);
    model->setData(mi, QString::fromStdString(data.id));

    mi = model->index(row, Server::PLAYERS);
    model->setData(mi, data.players);

    mi = model->index(row, Server::MAX_PLAYERS);
    model->setData(mi, data.maxPlayers);

    mi = model->index(row, Server::HOSTNAME);
    model->setData(mi, QString::fromStdString(data.hostname));

    mi = model->index(row, Server::MODNAME);
    model->setData(mi, QString::fromStdString(data.modname));

    mi = model->index(row, Server::VERSION);
    model->setData(mi, QString::fromStdString(data.version));

    mi = model->index(row, Server::PASSW);
    model->setData(mi, data.password);

    mi = model->index(row, Server::PING);
    model->setData(mi, PING_UNREACHABLE);
    PingHelper::Get().Add(row, {QString::fromStdString(data.address), data.port});
}
