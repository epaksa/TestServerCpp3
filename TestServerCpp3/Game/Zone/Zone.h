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
    void Initialize(const std::string& map_file_name);
    void Start();
    
    void PushPacketContext(std::shared_ptr<PacketContext> context);
    void SetServer(std::shared_ptr<Server> server);
    Concurrency::concurrent_queue<std::shared_ptr<PacketContext>>& GetPacketContextQueue();

private:
    void Run();

    void HandlePacketContext(std::shared_ptr<PacketContext> context);
    
    void ProcessPacket(cs_login* packet, std::shared_ptr<Client> client);
    void ProcessPacket(cs_move* packet, std::shared_ptr<Client> client);
    void ProcessPacket(cs_logout* packet, std::shared_ptr<Client> client);

    const bool CheckTile(const int x, const int y);
    void SetObject(const int x, const int y, Object* const object);
    void SetObject(const int current_x, const int current_y, const int next_x, const int next_y);
    void RemoveObject(const int x, const int y, const int object_id);
    const bool GetCurrentPos(OUT int& out_x, OUT int& out_y, const int object_id);

private:
    Concurrency::concurrent_queue<std::shared_ptr<PacketContext>> _packet_context_queue;
    std::shared_ptr<Server> _server;

    std::vector<std::vector<Tile>> _tile;
    std::map<int, Pos> _object_info;
    
    Random _random;
};

extern Zone ZONE;
