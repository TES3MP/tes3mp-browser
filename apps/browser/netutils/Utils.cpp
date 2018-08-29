//
// Created by koncord on 07.01.17.
//

#include <RakPeer.h>
#include <MessageIdentifiers.h>
#include <RakSleep.h>
#include <GetTime.h>

#include <sstream>

#include "Utils.hpp"

using namespace std;

unsigned int PingRakNetServer(const char *addr, unsigned short port)
{
    bool done = false;
    RakNet::TimeMS time = PING_UNREACHABLE;

    RakNet::SocketDescriptor socketDescriptor{0, ""};
    RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();
    peer->Startup(1, &socketDescriptor, 1);
    if (!peer->Ping(addr, port, false))
        return time;
    RakNet::TimeMS start = RakNet::GetTimeMS();
    while (!done)
    {
        RakNet::TimeMS now = RakNet::GetTimeMS();
        if (now - start >= PING_UNREACHABLE)
            break;

        RakNet::Packet *packet = peer->Receive();
        if (!packet)
            continue;

        switch (packet->data[0])
        {
            case ID_DISCONNECTION_NOTIFICATION:
            case ID_CONNECTION_LOST:
                done = true;
                break;
            case ID_CONNECTED_PING:
            case ID_UNCONNECTED_PONG:
            {
                RakNet::BitStream bsIn(&packet->data[1], packet->length, false);
                bsIn.Read(time);
                time = now - time;
                done = true;
                break;
            }
            default:
                break;
        }
        peer->DeallocatePacket(packet);
    }

    peer->Shutdown(0);
    RakNet::RakPeerInterface::DestroyInstance(peer);
    return time > PING_UNREACHABLE ? PING_UNREACHABLE : time;
}
