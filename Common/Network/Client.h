#pragma once

#include <boost/asio.hpp>
#include <queue>
#include "../DataStructure/RingBuffer.h"

struct BasePacket;

class Server;

class Client : public std::enable_shared_from_this<Client>
{
public:
    static std::shared_ptr<Client> Create(const int id, boost::asio::io_context& _io_context, std::shared_ptr<Server> server);

    void Receive();
    void Send(BasePacket& packet);
    void Send(std::shared_ptr<char> data, const int size);

    inline boost::asio::ip::tcp::socket& GetSocket() { return _socket; }
    inline const int GetID() { return _id; }

private:
    Client(const int id, boost::asio::io_context& _io_context, std::shared_ptr<Server> server);

    void OnReceive(const boost::system::error_code& error, const size_t bytes_transferred);
    void OnSend(const boost::system::error_code& error, const size_t bytes_transferred);
    void OnError(const std::string& error_message);

    const bool CanMakePacket(OUT char* packet_buffer, OUT int& packet_size);
    std::shared_ptr<BasePacket> MakePacket(const char* packet_buffer, const int packet_size);

private:
    int _id = 0;
    RingBuffer _read_buffer;
    RingBuffer _temp_buffer;
    std::shared_ptr<Server> _server;
    
    std::queue<std::shared_ptr<char>> _queue_send_buffer;
    std::recursive_mutex _mutex;

    boost::asio::ip::tcp::socket _socket;
    boost::asio::strand<boost::asio::io_context::executor_type> _strand;

    int receive_count = 0;
};