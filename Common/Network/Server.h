#pragma once

#include <list>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <concurrent_queue.h>
#include "Client.h"

struct BasePacket;

struct PacketContext
{
    std::shared_ptr<Client> _client = nullptr;
    std::shared_ptr<BasePacket> _packet = nullptr;
};

class Server : public std::enable_shared_from_this<Server>
{
public:
    Server(const int port, Concurrency::concurrent_queue<std::shared_ptr<PacketContext>>* const packet_context_queue);

    void Initialize(const int io_thread_count);
    void Listen();
    void Join();
    void Broadcast(BasePacket& packet, const int except_id = 0);

    void OnSocketError(const int client_id);

    void PushPacketContext(std::shared_ptr<PacketContext> context);

private:
    void OnAccept(std::shared_ptr<Client> client, const boost::system::error_code& error);
    void IOThreadRun();

private:
    std::list<std::shared_ptr<Client>> _list_client;
    int _next_client_id = 1;
    std::recursive_mutex _mutex;

    Concurrency::concurrent_queue<std::shared_ptr<PacketContext>>* _packet_context_queue = nullptr;

    // boost
    boost::asio::io_context _io_context;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _io_context_guard;

    boost::thread_group _pool_thread;
    boost::asio::ip::tcp::acceptor _acceptor;
};