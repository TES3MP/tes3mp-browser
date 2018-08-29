//
// Created by koncord on 07.01.17.
//

#include "qdebug.h"

#include "ServerInfoDialog.hpp"
#include <algorithm>
#include <utility>
#include <QThread>
#include "netutils/MasterClient.hpp"
#include "PingUpdater.hpp"

using namespace std;
using namespace RakNet;

ServerInfoDialog::ServerInfoDialog(const Server &server, QWidget *parent): QDialog(parent), serverData(server)
{
    setupUi(this);

    pingUpdater = new PingUpdater;

    connect(MasterClient::get(), &MasterClient::singleServer, this, &ServerInfoDialog::updateServer);
    connect(btnRefresh, &QPushButton::clicked, this, &ServerInfoDialog::refresh);

    FIX_UNTIL(make_version(2, 0), "Account system should be implemented");
    {
        lblstaT3MPMID->hide();
        leServerId->hide();

        lblstaReqT3MPMAcc->hide();
        cbReqT3MPMAcc->hide();
    }

    FIX_UNTIL(make_version(3, 0), "Download system should be implemented");
    {
        lblstaHasDLServ->hide();
        cbHasDLServer->hide();
    }
    btnConnect->setDisabled(true);
    setLogo(nullptr);
    initPingUpdater();
}

void ServerInfoDialog::initPingUpdater()
{
    pingThread = new QThread;
    pingUpdater = new PingUpdater;
    pingUpdater->moveToThread(pingThread);

    connect(pingThread, &QThread::started, pingUpdater, &PingUpdater::process);
    connect(pingUpdater, &PingUpdater::start, pingThread, &QThread::start);
    connect(pingUpdater, &PingUpdater::finished, pingThread, &QThread::quit);
    //connect(pingUpdater, SIGNAL(finished()), pingUpdater, SLOT(deleteLater()));
    connect(pingUpdater, &PingUpdater::updateModel, [this](int row, unsigned ping) {
        if (ping != PING_UNREACHABLE)
            this->lblPing->setText(QString::number(ping));
        else
            this->lblPing->setText(tr("Unreachable"));
        btnConnect->setDisabled(ping == PING_UNREACHABLE);
    });
}

ServerInfoDialog::~ServerInfoDialog()
{
    pingUpdater->stop();
}

bool ServerInfoDialog::isUpdated()
{
    return !serverData.id.empty();
}

void ServerInfoDialog::setLogo(QPixmap *image)
{
    QPixmap scaled;
    if (image == nullptr)
    {
        QPixmap pic(":/browser/tes3mp_logo.png");
        scaled = pic.scaled(192, 192, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    }
    else
        scaled = image->scaled(192, 192, Qt::IgnoreAspectRatio, Qt::FastTransformation);

    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(scaled));

    logoFrame->setAutoFillBackground(true);
    logoFrame->setPalette(palette);
    logoFrame->show();
}

void ServerInfoDialog::refresh()
{
    MasterClient::get()->requestServer(serverData);
}

int ServerInfoDialog::exec()
{
    refresh();
    return QDialog::exec();
}

void ServerInfoDialog::setPlayersCnt(int players, int _maxPlayers)
{
    if(_maxPlayers != -1)
        maxPlayers = _maxPlayers;
    lblPlayers->setText(QString::number(players) + " / " + QString::number(maxPlayers));
}


void ServerInfoDialog::updateServer(Server server, ServerExtra extra)
{
    qDebug() << "updateServer";
    lblName->setText(QString::fromStdString(server.hostname) + "\t" + QString::fromStdString(server.modname));
    leServerId->setText(QString::fromStdString(server.id));
    leAddr->setText(QString::fromStdString(server.address) + ":" + QString::number(server.port));
    setPlayersCnt(server.players, server.maxPlayers);

    cbHasDLServer->setChecked(!extra.dlServer.empty());

    listPlayers->clear();
    for (const auto &player : extra.players) // todo: optimize it
        listPlayers->addItem(QString::fromStdString(player));

    setPlayersCnt(extra.players.size());

    listPlugins->clear();
    for (const auto &plugin :  extra.plugins)
        listPlugins->addItem(QString::fromStdString(plugin));

    listRules->clear();
    for (const auto &[key, value] : extra.extraInfo)
    {
        std::string ruleStr = key;
        ruleStr += " : ";
        ruleStr += value;
        listRules->addItem(QString::fromStdString(ruleStr));
    }
}
