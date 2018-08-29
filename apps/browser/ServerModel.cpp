#include <qmessagebox.h>
#include "ServerModel.hpp"
#include <qdebug.h>
#include <apps/browser/netutils/Utils.hpp>

ServerModel::ServerModel(QObject *parent) : QAbstractTableModel(parent)
{
}
/*QHash<int, QByteArray> ServerModel::roleNames() const
{
    return roles;
}*/

QVariant ServerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() < 0 || index.row() > myData.size())
        return QVariant();

    const Server &sd = myData.at(index.row());

    if (role == Qt::DisplayRole)
    {
        QVariant var;
        switch (index.column())
        {
            case Server::ADDR:
                var = QString::fromStdString(sd.address);
                break;
            case Server::PORT:
                var = sd.port;
                break;
            case Server::ID:
                var = QString::fromStdString(sd.id);
                break;
            case Server::PASSW:
                var = sd.password ? "Yes" : "No";
                break;
            case Server::VERSION:
                var = QString::fromStdString(sd.version);
                break;
            case Server::PLAYERS:
                var = sd.players;
                break;
            case Server::MAX_PLAYERS:
                var = sd.maxPlayers;
                break;
            case Server::HOSTNAME:
                var = QString::fromStdString(sd.hostname);
                break;
            case Server::PING:
                var = sd.ping == PING_UNREACHABLE ? QVariant("Unreachable") : sd.ping;
                break;
            case Server::MODNAME:
                if (sd.modname.empty())
                    var = "Default";
                else
                    var = QString::fromStdString(sd.modname);
                break;
        }
        return var;
    }
    return QVariant();
}

QVariant ServerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var;
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::SizeHintRole)
        {
            /*if (section == ServerData::HOSTNAME)
                var = QSize(200, 25);*/
        }
        else if (role == Qt::DisplayRole)
        {

            switch (section)
            {
                case Server::ADDR:
                    var = "Address";
                    break;
                case Server::PORT:
                    var = "Port";
                    break;
                case Server::ID:
                    var = "Persistent id";
                    break;
                case Server::PASSW:
                    var = "Password";
                    break;
                case Server::VERSION:
                    var = "Version";
                    break;
                case Server::HOSTNAME:
                    var = "Host name";
                    break;
                case Server::PLAYERS:
                    var = "Players";
                    break;
                case Server::MAX_PLAYERS:
                    var = "Max players";
                    break;
                case Server::PING:
                    var = "Ping";
                    break;
                case Server::MODNAME:
                    var = "Game mode";
            }
        }
    }
    return var;
}

int ServerModel::rowCount(const QModelIndex &parent) const
{
    return myData.size();
}

int ServerModel::columnCount(const QModelIndex &parent) const
{
    return Server::LAST;
}

bool ServerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        int col = index.column();

        Server &sd = myData[row];
        bool ok = true;
        switch(col)
        {
            case Server::ADDR:
                sd.address = value.toString().toStdString();
                ok = !sd.address.empty();
                break;
            case Server::PORT:
                sd.port = static_cast<unsigned short>(value.toUInt(&ok));
                break;
            case Server::ID:
                sd.id = value.toString().toStdString();
                ok = !sd.id.empty();
                break;
            case Server::PASSW:
                sd.password = value.toBool();
                break;
            case Server::VERSION:
                sd.version = value.toString().toStdString();
                ok = !sd.address.empty();
                break;
            case Server::PLAYERS:
                sd.players = value.toUInt(&ok);
                break;
            case Server::MAX_PLAYERS:
                sd.maxPlayers = value.toUInt(&ok);
                break;
            case Server::HOSTNAME:
                sd.hostname = value.toString().toStdString();
                ok = !sd.address.empty();
                break;
            case Server::PING:
                sd.ping = value.toInt(&ok);
                break;
            case Server::MODNAME:
                sd.modname = value.toString().toStdString();
                break;
            default:
                return false;
        }
        if (ok)
            emit(dataChanged(index, index));
        return true;
    }
    return false;
}

bool ServerModel::insertRows(int position, int count, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + count - 1);

    myData.insert(position, count, {});

    endInsertRows();
    return true;
}

bool ServerModel::removeRows(int position, int count, const QModelIndex &parent)
{
    if (count == 0)
        return false;

    beginRemoveRows(parent, position, position + count - 1);
    myData.erase(myData.begin()+position, myData.begin() + position + count);
    endRemoveRows();

    return true;
}

QModelIndex ServerModel::index(int row, int column, const QModelIndex &parent) const
{

    QModelIndex index = QAbstractTableModel::index(row, column, parent);
    return index;
}
