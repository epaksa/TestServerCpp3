#include <Log/Log.h>
#include <Network/Server.h>
#include <format>

int main()
{
    Log::Start();

    std::shared_ptr<Server> server = std::make_shared<Server>(19001);
    server->Initialize(4);

    const std::string log = std::format("server start to listen.");
    Log::Write(log);

    server->Listen();
    server->Join();
}
