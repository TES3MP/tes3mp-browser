//
// Created by koncord on 01.07.18.
//

#pragma once

#include <QMetaType>
#include <variant>
#include <string>
#include <unordered_map>
#include <vector>
#include "Version.hpp"


struct Server
{
    std::string id;
    std::string address;
    unsigned short port;
    std::string modname, hostname, version;
    int players, maxPlayers;
    int ping;
    bool password;
    enum IDS
    {
        ID,
        ADDR,
        PORT,
        HOSTNAME,
        PLAYERS,
        MAX_PLAYERS,
        PASSW,
        MODNAME,
        PING,
        VERSION,
        LAST
    };
};

struct ServerExtra
{
    std::string dlServer;
    std::vector<std::string> players;
    std::vector<std::string> plugins;
    std::unordered_map<std::string, std::string> extraInfo;
};

Q_DECLARE_METATYPE(Server)
Q_DECLARE_METATYPE(ServerExtra)
