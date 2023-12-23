#include "Server.h"
#include "Client.h"
#include "../Log/Log.h"
#include "Packet/Packet.h"

Server::Server(const int port, const int io_thread_count) :
    _io_context_guard(boost::asio::make_work_guard<boost::asio::io_context>(_io_context)), _acceptor(_io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    for (int i = 0; i < io_thread_count; ++i)
    {
        _pool_thread.create_thread(boost::bind(&boost::asio::io_context::run, &_io_context));
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
    static int count = 0;

    char* c = new char[BUFFER_SIZE] {0};
    std::shared_ptr<char> send_buffer(c);

    const int data_size = packet.Serialize(OUT send_buffer.get());

    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        for (const std::shared_ptr<Client> client : _list_client)
        {
            client->Send(send_buffer, data_size);
            ++count;
        }

        std::string log = std::format("count = {}", count);
        Log::Write(log);
    }
}

void Server::OnSocketError(const int error_client_id)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    _list_client.remove_if([=](const std::shared_ptr<Client> client) {
        return client->GetID() == error_client_id;
        });
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

        Listen();
    }
    else
    {
        accept_log = std::format("[client id {}] error => {}", client->GetID(), error.message());
    }

    Log::Write(accept_log);
}
