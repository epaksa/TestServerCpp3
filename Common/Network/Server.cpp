#include "Server.h"
#include "../Log/Log.h"
#include "Packet/Packet.h"

Server::Server(const int port, Concurrency::concurrent_queue<std::shared_ptr<PacketContext>>* const packet_context_queue) :
    _packet_context_queue(packet_context_queue),
    _io_context_guard(boost::asio::make_work_guard<boost::asio::io_context>(_io_context)),
    _acceptor(_io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    
}

void Server::Initialize(const int io_thread_count)
{
    for (int i = 0; i < io_thread_count; ++i)
    {
        _pool_thread.create_thread(boost::bind(&Server::IOThreadRun, shared_from_this()));
    }
}

void Server::Listen()
{
    std::shared_ptr<Client> client = nullptr;

    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        client = Client::Create(_next_client_id, _io_context, shared_from_this());
    }

    _acceptor.async_accept(client->GetSocket(), boost::bind(&Server::OnAccept, shared_from_this(), client, boost::asio::placeholders::error));
}

void Server::Join()
{
    _pool_thread.join_all();
}

void Server::Broadcast(BasePacket& packet)
{
    char* c = new char[BUFFER_SIZE] {0};
    std::shared_ptr<char> send_buffer(c);

    const int data_size = packet.Serialize(OUT send_buffer.get());

    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        for (const std::shared_ptr<Client> client : _list_client)
        {
            client->Send(send_buffer, data_size);
        }
    }
}

void Server::OnSocketError(const int error_client_id)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    _list_client.remove_if([=](const std::shared_ptr<Client> client) {
        return client->GetID() == error_client_id;
        });
}

void Server::PushPacketContext(std::shared_ptr<PacketContext> context)
{
    if (nullptr == _packet_context_queue)
    {
        Log::Write("_packet_context_queue is null.");
        return;
    }

    _packet_context_queue->push(context);
}

void Server::OnAccept(std::shared_ptr<Client> client, const boost::system::error_code& error)
{
    std::string accept_log;

    if (!error)
    {
        client->Receive();

        accept_log = std::format("[client id {}] accepted.", client->GetID());

        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);

            _list_client.push_back(client);
            ++_next_client_id;
        }

        {
            std::shared_ptr<PacketContext> context = std::make_shared<PacketContext>();
            
            std::shared_ptr<cs_login> packet = std::make_shared<cs_login>();
            packet->_client_id = client->GetID();

            context->_client = client;
            context->_packet = packet;

            PushPacketContext(context);
        }

        Listen();
    }
    else
    {
        accept_log = std::format("[client id {}] error => {}", client->GetID(), error.message());
    }

    Log::Write(accept_log);
}

void Server::IOThreadRun()
{
    while (true)
    {
        try
        {
            _io_context.run();
            break; // run() exited normally
        }
        catch (std::exception const& e)
        {
            std::string exception_log = std::format("asio exception. msg : {}", e.what());
            Log::Write(exception_log);
        }
    }
}
