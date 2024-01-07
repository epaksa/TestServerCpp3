#include "Zone.h"
#include "Tile.h"
#include <fstream>
#include <thread>

Zone ZONE;
Concurrency::concurrent_queue<std::shared_ptr<BasePacket>> Zone::_packet_queue;
std::vector<std::vector<Tile>> Zone::_tile;

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
    std::shared_ptr<BasePacket> packet;

    while (true)
    {
        if (_packet_queue.try_pop(OUT packet))
        {
            HandlePacket(packet);
        }
    }
}

void Zone::Start()
{
    std::thread zone_thread(&Zone::Run);
    zone_thread.detach();
}

void Zone::PushPacket(std::shared_ptr<BasePacket> packet)
{
    _packet_queue.push(packet);
}

void Zone::HandlePacket(std::shared_ptr<BasePacket> packet)
{
    switch (packet->_packet_id)
    {
    case PacketID::cs_login:
        ProcessPacket((cs_login*)packet.get());
        break;
    case PacketID::cs_move:
        ProcessPacket((cs_move*)packet.get());
        break;
    default:
        break;
    }
}

void Zone::ProcessPacket(cs_login* packet)
{
}

void Zone::ProcessPacket(cs_move* packet)
{
}
