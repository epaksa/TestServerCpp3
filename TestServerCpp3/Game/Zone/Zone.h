#pragma once

#include <concurrent_queue.h>
#include <string>
#include <vector>
#include <Network/Packet/Packet.h>
#include "Tile.h"

class Zone
{
public:
    static void Initialize(const std::string& map_file_name);
    static void Start();
    static void PushPacket(std::shared_ptr<BasePacket> packet);

private:
    static void Run();

    static void HandlePacket(std::shared_ptr<BasePacket> packet);
    static void ProcessPacket(cs_login* packet);
    static void ProcessPacket(cs_move* packet);

private:
    static Concurrency::concurrent_queue<std::shared_ptr<BasePacket>> _packet_queue;

    static std::vector<std::vector<Tile>> _tile;
};

extern Zone ZONE;
