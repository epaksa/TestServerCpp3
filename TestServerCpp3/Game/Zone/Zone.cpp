#include "Zone.h"
#include "Tile.h"
#include "../Object/Object.h"
#include "../Object/Player.h"
#include <fstream>
#include <thread>
#include <Log/Log.h>
#include <Network/Server.h>

Zone ZONE;
Concurrency::concurrent_queue<std::shared_ptr<PacketContext>> Zone::_packet_context_queue;
std::shared_ptr<Server> Zone::_server = nullptr;
std::vector<std::vector<Tile>> Zone::_tile;
Random Zone::_random;

void Zone::Initialize(const std::string& map_file_name)
{
    std::ifstream stream(map_file_name);

    std::string str;

    while (std::getline(stream, str))
    {
        std::vector<Tile> list_tile;

        size_t pos = 0;
        while ((pos = str.find(",")) != std::string::npos)
        {
            Tile tile;
            
            const int tile_data_int = std::stoi(str.substr(0, pos));

            if (-1 == tile_data_int)
            {
                tile._type = TileType::road;
            }
            else
            {
                tile._type = TileType::wall;
            }

            list_tile.push_back(tile);

            str.erase(0, pos + 1);
        }

        {
            Tile tile;

            const int tile_data_int = std::stoi(str.substr(0, pos));

            if (-1 == tile_data_int)
            {
                tile._type = TileType::road;
            }
            else
            {
                tile._type = TileType::wall;
            }

            list_tile.push_back(tile);
        }

        _tile.push_back(list_tile);
    }
}

void Zone::Run()
{
    std::shared_ptr<PacketContext> context;

    while (true)
    {
        if (_packet_context_queue.try_pop(OUT context))
        {
            HandlePacketContext(context);
        }
    }
}

void Zone::Start()
{
    std::thread zone_thread(&Zone::Run);
    zone_thread.detach();
}

void Zone::PushPacketContext(std::shared_ptr<PacketContext> context)
{
    _packet_context_queue.push(context);
}

void Zone::SetServer(std::shared_ptr<Server> server)
{
    _server = server;
}

Concurrency::concurrent_queue<std::shared_ptr<PacketContext>>& Zone::GetPacketContextQueue()
{
    return _packet_context_queue;
}

void Zone::HandlePacketContext(std::shared_ptr<PacketContext> context)
{
    // cs 패킷 보낸 client한테만 응답해야하면, context안에 client를 Process~()함수로 넘겨주기

    switch (context->_packet->_packet_id)
    {
    case PacketID::cs_login:
        ProcessPacket((cs_login*)context->_packet.get(), context->_client);
        break;
    case PacketID::cs_move:
        ProcessPacket((cs_move*)context->_packet.get());
        break;
    default:
        break;
    }
}

void Zone::ProcessPacket(cs_login* packet, std::shared_ptr<Client> client)
{
    if (nullptr == _server)
    {
        Log::Write("_server is null.");
        return;
    }

    bool found_pos = false;
    int x = 0;
    int y = 0;

    while (false == found_pos)
    {
        y = _random.Get(0, _tile.size() - 1);
        x = _random.Get(0, _tile[y].size() - 1);

        found_pos = CheckTile(x, y);
    }

    Player* player = new Player(packet->_client_id);
    
    if (false == SetTile(x, y, player))
    {
        delete player;
    }

    sc_login response;
    response._client_id = packet->_client_id;
    response._x = x;
    response._y = y;

    const std::string log = std::format("cs_login => id : {}. x : {}, y : {}", packet->_client_id, x, y);
    Log::Write(log);

    client->Send(response);

    sc_welcome broadcast_packet;
    broadcast_packet._client_id = packet->_client_id;
    broadcast_packet._x = x;
    broadcast_packet._y = y;

    _server->Broadcast(broadcast_packet);
}

void Zone::ProcessPacket(cs_move* packet)
{
}

const bool Zone::CheckTile(const int x, const int y)
{
    if (y < 0 || y >= _tile.size())
    {
        const std::string error_log = std::format("not valid pos. x : {}, y : {}", x, y);
        Log::Write(error_log);
        return false;
    }

    if (x < 0 || x >= _tile[y].size())
    {
        const std::string error_log = std::format("not valid pos. x : {}, y : {}", x, y);
        Log::Write(error_log);
        return false;
    }

    return _tile[x][y].IsEmpty();
}

const bool Zone::SetTile(const int x, const int y, Object* const object)
{
    if (nullptr != _tile[x][y]._object)
    {
        const std::string error_log = std::format("object exists. x : {}, y : {}", x, y);
        Log::Write(error_log);
        return false;
    }

    _tile[x][y]._object = object;

    return true;
}
