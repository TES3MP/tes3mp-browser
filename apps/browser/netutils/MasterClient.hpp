//
// Created by koncord on 19.07.18.
//

#pragma once

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtNetwork>
#include <QUrl>
#include <QtWebSockets>

#include <apps/browser/Data.hpp>

class SocketWorker;

class MasterClient: public QObject
{
    Q_OBJECT
public:
    static void create(QObject *parent);
    static MasterClient *get();

    void address(const QString &addr);

    void requestList();
    void requestServer(const Server &server);
    void requestLatestVersionStr();

    ~MasterClient();
protected:
    explicit MasterClient(QObject *parent);

    QString addr;
    QThread *thread;
    SocketWorker *worker;
    QNetworkAccessManager *nam;
private slots:
    void error(QAbstractSocket::SocketError);

signals:
    void server(Server server);
    void finished();

    void singleServer(Server server, ServerExtra extra);
    void latestVersion(const QString &version);
private:
    static MasterClient *mThis;
};

class SocketWorker : public QObject
{
Q_OBJECT
public:
    SocketWorker();
    ~SocketWorker();
    void requestExtra(const Server &server);
    void requestList();
    void address(const QString &addr);
public slots:
    void process();
    void abort();
signals:
    void parsedServer(Server server);
    void serverExtra(Server server, ServerExtra extra);
    void finished();
private:
    QUrl addr;
    QWebSocket *webSocket;
    QString serverId;
};
