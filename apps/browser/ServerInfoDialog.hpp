//
// Created by koncord on 07.01.17.
//

#pragma once

#include "ui_ServerInfo.h"
#include <apps/browser/netutils/Utils.hpp>
#include <RakNetTypes.h>
#include "Data.hpp"

class PingUpdater;

class ServerInfoDialog : public QDialog,  public Ui::Dialog
{
    Q_OBJECT
public:
    explicit ServerInfoDialog(const Server &server, QWidget *parent = nullptr);
    ~ServerInfoDialog() override;
    bool isUpdated();
public slots:
    void refresh();
    int exec() Q_DECL_OVERRIDE;
    void setLogo(QPixmap *image);
    void updateServer(Server server, ServerExtra extra);
signals:
    void updatedInfo(const Server &server, const ServerExtra &extra);
private:
    void initPingUpdater();
    void setPlayersCnt(int players, int maxPlayers = -1);
private:
    Server serverData;
    ServerExtra extraData;

    PingUpdater *pingUpdater;
    QThread *pingThread;

    int maxPlayers;
};
