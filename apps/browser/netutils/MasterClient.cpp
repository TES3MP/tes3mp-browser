//
// Created by koncord on 19.07.18.
//

#include "MasterClient.hpp"
#include <QtNetwork/QTcpSocket>
#include <QMessageBox>

#include <utility>

#include <json.hpp>


MasterClient::MasterClient(QObject *parent) : QObject(parent)
{
    //thread = new QThread;
    worker = new SocketWorker;

    connect(worker, &SocketWorker::parsedServer, [this](Server server) {
        emit this->server(std::move(server));
    });

    connect(worker, &SocketWorker::finished, this, &MasterClient::finished);


    connect(worker, &SocketWorker::serverExtra, [this](Server server, ServerExtra extra) {
        emit this->singleServer(std::move(server), std::move(extra));
    });

    /*connect(thread, &QThread::finished, [this]() {
        //worker->deleteLater();
        qDebug() << "thread finished";
    });*/

    //connect(worker, &SocketWorker::finished, thread, &QThread::quit);

    //worker->moveToThread(thread);

    nam = new QNetworkAccessManager(this);
}

MasterClient::~MasterClient()
{

}

MasterClient *MasterClient::mThis = nullptr;

void MasterClient::create(QObject *parent)
{
    if (!mThis)
        mThis = new MasterClient(parent);
}

MasterClient *MasterClient::get()
{
    return mThis;
}

void MasterClient::address(const QString &address)
{
    addr = address;
    worker->address(address);
}

void MasterClient::error(QAbstractSocket::SocketError socketError)
{
    qDebug() << "err: " << socketError;
}

void MasterClient::requestList()
{
    /*if (thread)
    {
        thread->quit();
        thread->wait();
    }

    disconnect(thread, &QThread::started, nullptr, nullptr);

    connect(thread, &QThread::started, [this]() {
        worker->requestList();
        worker->process();
    });

    thread->start();*/

    worker->requestList();
    worker->process();
}

void MasterClient::requestServer(const Server &server)
{

    /*thread->quit();
    thread->wait();


    disconnect(thread, &QThread::started, nullptr, nullptr);

    connect(thread, &QThread::started, [this, server]() {
        worker->requestExtra(server);
        worker->process();
    });

    thread->start();*/

    worker->requestExtra(server);
    worker->process();
}


void MasterClient::requestLatestVersionStr()
{
    nam->get(QNetworkRequest(QUrl("https://" + addr + "/browser/version")));
    connect(nam, &QNetworkAccessManager::finished,
            [this](QNetworkReply *reply) {
                QString version = reply->readAll();
                emit latestVersion(version);
                disconnect(nam, &QNetworkAccessManager::finished, nullptr, nullptr);
            });
}

SocketWorker::SocketWorker() : webSocket(nullptr)
{
}

SocketWorker::~SocketWorker()
{
    qDebug() << "SocketWorker::~SocketWorker()";
}

void SocketWorker::address(const QString &_addr)
{
    addr = "wss://" + _addr + "/websocket";
}

void SocketWorker::requestExtra(const Server &server)
{
    serverId = QString::fromStdString(server.id);
}

void SocketWorker::requestList()
{
    serverId.clear();
}

bool OnRequestError(nlohmann::json &json)
{
    if (json.count("status") == 1)
    {
        auto status = QString::fromStdString(json["status"]);
        auto msg = QString::fromStdString(json["message"]);
        QMessageBox::critical(nullptr, status, msg);
        return true;
    }
    return false;
}

Server getBasic(const nlohmann::json &json, bool extra = false)
{
    Server server;
    server.modname = json["modname"];
    server.hostname = json["hostname"];
    server.version = json["version"];

    if (server.version.empty())
        server.version = "Unknown";

    server.maxPlayers = json["maxPlayers"];
    server.port = json["port"];
    server.address = json["address"];
    server.password = json["passw"];
    if (!extra)
    {
        server.id = json["id"];
        server.players = json["players"];
    }

    return server;
}

void SocketWorker::process()
{
    auto fnSend = [this]() {
        QString cmd;
        if (serverId.isEmpty())
            cmd = "GET SERVERS";
        else
            cmd = "GET SERVER " + serverId;

        qDebug() << cmd;
        qint64 bytesSent = webSocket->sendTextMessage(cmd);

        if (bytesSent == 0)
            qDebug() << "Not connected";
        else
            qDebug() << "Bytes sent:" << bytesSent;
        return bytesSent;
    };

    if(!webSocket)
    {
        webSocket = new QWebSocket;
        //connect(webSocket, &QWebSocket::disconnected, this, &SocketWorker::finished);
        connect(webSocket, &QWebSocket::disconnected, [this]() {
            qDebug() << "Disconnected" << webSocket->closeReason();
        });

        connect(webSocket, &QWebSocket::binaryMessageReceived, [this](const QByteArray &message) {
            if (serverId.isEmpty())
                return;

            qDebug() << "binaryMessageReceived";

            nlohmann::json json = nlohmann::json::from_cbor(message.data(), message.size());

            if (OnRequestError(json) || json.empty())
                return;

            Server server = getBasic(json, true);
            ServerExtra extra;

            extra.dlServer = json["dlServer"];

            for (const auto &player : json["players"])
                extra.players.emplace_back(player);

            for (const auto &plugin : json["plugins"])
                extra.plugins.emplace_back(plugin);

            for (const auto &item : json["extraInfo"].items())
                extra.extraInfo[item.key()] = item.value();

            emit serverExtra(std::move(server), std::move(extra));
        });

        connect(webSocket, &QWebSocket::binaryFrameReceived, [this](const QByteArray &frame, bool last) {
            if (!serverId.isEmpty())
                return;
            nlohmann::json json = nlohmann::json::from_cbor(frame.data(), frame.size());

            if (!(OnRequestError(json) || json.empty()))
            {
                Server server = getBasic(json);
                emit parsedServer(std::move(server));
            }

            if(last)
                emit finished(); // emit finished if frame is last
        });

        connect(webSocket, &QWebSocket::connected, fnSend);
    }

    if (fnSend() == 0)
        webSocket->open(addr); // we are not connected
}

void SocketWorker::abort()
{
    webSocket->close();
}
