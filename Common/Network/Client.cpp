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
    _temp_buffer.Clear();
    _read_buffer.Clear();
}

void Client::Receive()
{
    // clear하고, temp buffer를 쓰는거면.. 이름만 ring buffer지 ring buffer를 쓰는게 아니야.. 현재 들어있는 data양만큼 read buffer에 +시켜준걸 read some할때 넘겨줘야하지 않나..
    // 일단 temp 버퍼로 packet처리하는거 확인하고나서 고치자. 그리고...
    /*
    - common에서 다 처리하고 있는거 고치고 싶음
        - 그럼 1 map이라도 login 처리하는 thread가 있어야함
            - 지금은 그냥 broadcast였다. network thread에서 그냥 처리하는거
                이걸 login thread 처리하는걸로 바꿔야 됨.
                packet class들어오니까 볼륨이 어느정도 있네..
    - 일단 common에서 packet처리하는거 만들고
    - packet 받고 broadcast까지 확인 후에, logic thread 처리 시작하기
    */
    _read_buffer.Clear();
    _socket.async_read_some(boost::asio::buffer((void*)_read_buffer.Data(), BUFFER_SIZE - 1),
        boost::asio::bind_executor(_strand, boost::bind(&Client::OnReceive, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
}

void Client::Send(BasePacket& packet)
{
    std::shared_ptr<char> send_buffer = std::make_shared<char>();
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

        std::vector<std::shared_ptr<BasePacket>> list_packet;

        while (remain_bytes > 0)
        {
            if (false == CanMakePacket(OUT packet_buffer, OUT packet_size))
            {
                _temp_buffer.Clear();
                _temp_buffer.Copy(_read_buffer);
                break;
            }

            // worker thread로 원래는 넘어가야하고, buffer 및 packet instance life cycle에 대한 고민을 해야함
            std::shared_ptr<BasePacket> packet = MakePacket(packet_buffer, packet_size);

            if (nullptr != packet)
            {
                list_packet.push_back(packet);
            }
            else
            {
                Log::Write("MakePacket() returned nullptr.");
            }

            memset(packet_buffer, 0, BUFFER_SIZE);

            remain_bytes -= packet_size;
        }

        Receive();

        // worker thread로 원래는 넘어가야하는 내용임
        for (const std::shared_ptr<BasePacket> packet : list_packet)
        {
            sc_move move_packet;
            move_packet._move_client_id = _id;
            move_packet._x = ((cs_move*)packet.get())->_x;
            move_packet._y = ((cs_move*)packet.get())->_y;

            // commented for broadcast test
            const std::string receive_log = std::format("[client id {}] received. read bytes : {}, packet id : {}, x : {}, y : {}", _id, bytes_transferred, (int)packet->_packet_id, move_packet._x, move_packet._y);

            Log::Write(receive_log);

            _server->Broadcast(move_packet);
        }
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

    std::string log;

    if (!error)
    {
        // commented for broadcast test
        /*log = std::format("[client id {}] sent. sent bytes : {}", _id, bytes_transferred);

        Log::Write(log);*/
    }
    else
    {
        log = std::format("[client id {}] sent error => {}", _id, error.message());

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

    if (false == _temp_buffer.Empty())
    {
        Log::Write("temp buffer is not empty!!");

        if (false == _temp_buffer.Pop(OUT packet_buffer, sizeof(int)))
        {
            Log::Write("get sizeof(int) failed in temp buffer.");
            return false;
        }

        current_size = sizeof(int);
        current_size += _temp_buffer.PopAll(OUT packet_buffer + current_size);
    }
    else
    {
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
    }

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
