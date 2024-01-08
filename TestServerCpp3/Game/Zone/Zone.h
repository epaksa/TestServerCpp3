#pragma once

#include <concurrent_queue.h>
#include <string>
#include <vector>
#include <Network/Packet/Packet.h>
#include <Etc/Random.h>
#include "Tile.h"

struct PacketContext;

class Server;
class Object;

class Zone
{
public:
    static void Initialize(const std::string& map_file_name);
    static void Start();
    
    static void PushPacketContext(std::shared_ptr<PacketContext> context);
    static void SetServer(std::shared_ptr<Server> server);
    static Concurrency::concurrent_queue<std::shared_ptr<PacketContext>>& GetPacketContextQueue();

private:
    static void Run();

    static void HandlePacketContext(std::shared_ptr<PacketContext> context);
    static void ProcessPacket(cs_login* packet);
    static void ProcessPacket(cs_move* packet);

    static const bool CheckTile(const int x, const int y);
    static const bool SetTile(const int x, const int y, Object* const object);

private:
    static Concurrency::concurrent_queue<std::shared_ptr<PacketContext>> _packet_context_queue;
    static std::shared_ptr<Server> _server;

    static std::vector<std::vector<Tile>> _tile;
    static Random _random;
};

extern Zone ZONE;
