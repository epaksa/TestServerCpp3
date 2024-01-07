#include <Log/Log.h>
#include <Network/Server.h>
#include <format>
#include "Game/Zone/Zone.h"

int main()
{
    std::string log_file_name = "log.txt";
    std::string map_file_name = "map.csv";

    Log::Start(log_file_name);

    ZONE.Initialize(map_file_name);
    ZONE.Start();

    std::shared_ptr<Server> server = std::make_shared<Server>(19001);
    server->Initialize(4);

    const std::string log = std::format("server start to listen.");
    Log::Write(log);

    server->Listen();
    server->Join();
}
