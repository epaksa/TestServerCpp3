#pragma once

#include <concurrent_queue.h>
#include <string>
#include <vector>
#include <map>
#include <Network/Packet/Packet.h>
#include <Etc/Random.h>
#include "Tile.h"

struct PacketContext;

class Client;
class Server;
class Object;

struct Pos
{
    int _x = 0;
    int _y = 0;

    Pos(const int x, const int y) : _x(x), _y(y) {}
};

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
    
    static void ProcessPacket(cs_login* packet, std::shared_ptr<Client> client);
    static void ProcessPacket(cs_move* packet, std::shared_ptr<Client> client);
    static void ProcessPacket(cs_logout* packet, std::shared_ptr<Client> client);

    static const bool CheckTile(const int x, const int y);
    static void SetObject(const int x, const int y, Object* const object);
    static void SetObject(const int current_x, const int current_y, const int next_x, const int next_y);
    static void RemoveObject(const int x, const int y, const int object_id);
    static const bool GetCurrentPos(OUT int& out_x, OUT int& out_y, const int object_id);

private:
    static Concurrency::concurrent_queue<std::shared_ptr<PacketContext>> _packet_context_queue;
    static std::shared_ptr<Server> _server;

    static std::vector<std::vector<Tile>> _tile;
    static std::map<int, Pos> _object_info;
    
    static Random _random;
};

extern Zone ZONE;
