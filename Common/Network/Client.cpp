#include <boost/bind/bind.hpp>
#include "Client.h"
#include "Server.h"
#include "../Log/Log.h"
#include "Packet/Packet.h"

std::shared_ptr<Client> Client::Create(const int id, boost::asio::io_context& _io_context, std::shared_ptr<Server> server)
{
    return std::shared_ptr<Client>(new Client(id, _io_context, server));
}

Client::Client(const int id, boost::asio::io_context& _io_context, std::shared_ptr<Server> server) : _id(id), _socket(_io_context), _strand(boost::asio::make_strand(_io_context)), _server(server)
{
    _read_buffer.Clear();
}

void Client::Receive()
{
    _socket.async_read_some(boost::asio::buffer((void*)_read_buffer.IndexToWrite(), _read_buffer.AvailableSize()),
        boost::asio::bind_executor(_strand, boost::bind(&Client::OnReceive, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
}

void Client::Send(BasePacket& packet)
{
    char* c = new char[BUFFER_SIZE] {0};
    std::shared_ptr<char> send_buffer(c);

    const int data_size = packet.Serialize(OUT send_buffer.get());

    Send(send_buffer, data_size);
}

void Client::Send(std::shared_ptr<char> data, const int size)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    _queue_send_buffer.push(data);

    boost::asio::async_write(_socket, boost::asio::buffer(_queue_send_buffer.back().get(), size), boost::bind(&Client::OnSend, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Client::OnReceive(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if (!error)
    {
        if (false == _read_buffer.SetWriteIndex((int)bytes_transferred))
        {
            const std::string error_log = std::format("[client id {}] set write index error. bytes_transferred : {}", _id, bytes_transferred);

            Log::Write(error_log);
            return;
        }

        char packet_buffer[BUFFER_SIZE]{ 0 };
        int packet_size = 0;
        int remain_bytes = (int)bytes_transferred;

        while (remain_bytes > 0)
        {
            if (false == CanMakePacket(OUT packet_buffer, OUT packet_size))
            {
                break;
            }

            std::shared_ptr<BasePacket> packet = MakePacket(packet_buffer, packet_size);

            if (nullptr != packet)
            {
                std::shared_ptr<PacketContext> context = std::make_shared<PacketContext>();

                context->_client = shared_from_this();
                context->_packet = packet;

                _server->PushPacketContext(context);
            }
            else
            {
                Log::Write("MakePacket() returned nullptr.");
            }

            memset(packet_buffer, 0, BUFFER_SIZE);

            remain_bytes -= packet_size;
        }

        Receive();
    }
    else
    {
        const std::string error_log = std::format("[client id {}] receive error => {}", _id, error.message());

        OnError(error_log);
    }
}

void Client::OnSend(const boost::system::error_code& error, const size_t bytes_transferred)
{
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        _queue_send_buffer.pop();
    }

    if (error)
    {
        std::string log = std::format("[client id {}] sent error => {}", _id, error.message());

        OnError(log);
    }
}

void Client::OnError(const std::string& error_message)
{
    Log::Write(error_message);

    try
    {
        _socket.close();
        _server->OnSocketError(_id);
    }
    catch (const std::exception& ex)
    {
        std::string exception_log = std::format("[client id {}] exception occurs. message => {}", _id, ex.what());

        Log::Write(exception_log);
    }
}

const bool Client::CanMakePacket(OUT char* packet_buffer, OUT int& packet_size)
{
    int current_size = 0;

    if (_read_buffer.Empty())
    {
        Log::Write("read buffer empty.");
        return false;
    }

    if (false == _read_buffer.Pop(OUT packet_buffer, sizeof(int)))
    {
        Log::Write("get sizeof(int) failed in read buffer.");
        return false;
    }

    current_size = sizeof(int);

    packet_size = packet_buffer[0];

    if (false == _read_buffer.Pop(OUT packet_buffer + current_size, packet_size - current_size))
    {
        std::string error_log = std::format("get (packet_size({}) - current_size({})) failed in read buffer.", packet_size, current_size);

        Log::Write(error_log);
        return false;
    }

    return true;
}

std::shared_ptr<BasePacket> Client::MakePacket(const char* packet_buffer, const int packet_size) // 지금은 packet_size를 안써도, 가변 size의 패킷들이 있어서 나중에 필요함.
{
    // todo : 생성자에 size집어넣는거 빼기. size는 보낼때 결정되야함... list형식이나 chat같은 패킷땜에

    PacketID packet_id = (PacketID)packet_buffer[sizeof(int)];

    std::shared_ptr<BasePacket> packet = nullptr;

    switch (packet_id)
    {
    case PacketID::cs_move:
        packet = std::make_shared<cs_move>();
        packet->Deserialize(packet_buffer);
        break;
    default:
        break;
    }

    return packet;
}
