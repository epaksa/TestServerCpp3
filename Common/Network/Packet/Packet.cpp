#include "Packet.h"
#include <corecrt_memcpy_s.h>

const int BasePacket::Serialize(OUT char* result)
{
    _size += (sizeof(BasePacket) - sizeof(void*));

    int result_size = 0;

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_size, sizeof(_size));
    result_size += sizeof(_size);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_packet_id, sizeof(_packet_id));
    result_size += sizeof(_packet_id);

    return result_size;
}

const int BasePacket::Deserialize(const char* data)
{
    int result_size = 0;

    memcpy_s(&_size, sizeof(_size), (void*)(data + result_size), sizeof(_size));
    result_size += sizeof(_size);

    memcpy_s(&_packet_id, sizeof(_packet_id), (void*)(data + result_size), sizeof(_packet_id));
    result_size += sizeof(_packet_id);

    return result_size;
}

const int cs_login::Serialize(OUT char* result)
{
    _size += (sizeof(cs_login) - sizeof(void*)) - (sizeof(BasePacket) - sizeof(void*)); // sizeof(void*) => size of virtual function table pointer

    int result_size = 0;

    result_size += __super::Serialize(result);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_client_id, sizeof(_client_id));
    result_size += sizeof(_client_id);

    return result_size;
}

const int cs_login::Deserialize(const char* data)
{
    int result_size = 0;

    result_size += __super::Deserialize(data);

    memcpy_s(&_client_id, sizeof(_client_id), (void*)(data + result_size), sizeof(_client_id));
    result_size += sizeof(_client_id);

    return result_size;
}

const int sc_login::Serialize(OUT char* result)
{
    _size += sizeof(ClientPos); // size of _my_info
    _size += sizeof(int); // size of _list_client
    _size += sizeof(ClientPos) * (int)_list_client.size(); // size of _list_client

    int result_size = 0;

    result_size += __super::Serialize(result);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_my_info._client_id, sizeof(_my_info._client_id));
    result_size += sizeof(_my_info._client_id);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_my_info._x, sizeof(_my_info._x));
    result_size += sizeof(_my_info._x);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_my_info._y, sizeof(_my_info._y));
    result_size += sizeof(_my_info._y);

    int _list_client_size = (int)_list_client.size();
    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_list_client_size, sizeof(_list_client_size));
    result_size += sizeof(_list_client_size);

    for (const ClientPos& data_in_list : _list_client)
    {
        memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&data_in_list._client_id, sizeof(data_in_list._client_id));
        result_size += sizeof(data_in_list._client_id);

        memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&data_in_list._x, sizeof(data_in_list._x));
        result_size += sizeof(data_in_list._x);

        memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&data_in_list._y, sizeof(data_in_list._y));
        result_size += sizeof(data_in_list._y);
    }

    return result_size;
}

const int sc_login::Deserialize(const char* data)
{
    int result_size = 0;

    result_size += __super::Deserialize(data);

    memcpy_s(&_my_info._client_id, sizeof(_my_info._client_id), (void*)(data + result_size), sizeof(_my_info._client_id));
    result_size += sizeof(_my_info._client_id);

    memcpy_s(&_my_info._x, sizeof(_my_info._x), (void*)(data + result_size), sizeof(_my_info._x));
    result_size += sizeof(_my_info._x);

    memcpy_s(&_my_info._y, sizeof(_my_info._y), (void*)(data + result_size), sizeof(_my_info._y));
    result_size += sizeof(_my_info._y);

    size_t _list_client_size = 0;
    memcpy_s(&_list_client_size, sizeof(_list_client_size), (void*)(data + result_size), sizeof(_list_client_size));
    result_size += sizeof(_list_client_size);

    for (int i = 0; i < _list_client_size; ++i)
    {
        ClientPos data_in_list;

        memcpy_s(&data_in_list._client_id, sizeof(data_in_list._client_id), (void*)(data + result_size), sizeof(data_in_list._client_id));
        result_size += sizeof(data_in_list._client_id);

        memcpy_s(&data_in_list._x, sizeof(data_in_list._x), (void*)(data + result_size), sizeof(data_in_list._x));
        result_size += sizeof(data_in_list._x);

        memcpy_s(&data_in_list._y, sizeof(data_in_list._y), (void*)(data + result_size), sizeof(data_in_list._y));
        result_size += sizeof(data_in_list._y);

        _list_client.push_back(data_in_list);
    }

    return result_size;
}

const int sc_welcome::Serialize(OUT char* result)
{
    _size += (sizeof(sc_welcome) - sizeof(void*)) - (sizeof(BasePacket) - sizeof(void*));

    int result_size = 0;

    result_size += __super::Serialize(result);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_client_id, sizeof(_client_id));
    result_size += sizeof(_client_id);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_x, sizeof(_x));
    result_size += sizeof(_x);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_y, sizeof(_y));
    result_size += sizeof(_y);

    return result_size;
}

const int sc_welcome::Deserialize(const char* data)
{
    int result_size = 0;

    result_size += __super::Deserialize(data);

    memcpy_s(&_client_id, sizeof(_client_id), (void*)(data + result_size), sizeof(_client_id));
    result_size += sizeof(_client_id);

    memcpy_s(&_x, sizeof(_x), (void*)(data + result_size), sizeof(_x));
    result_size += sizeof(_x);

    memcpy_s(&_y, sizeof(_y), (void*)(data + result_size), sizeof(_y));
    result_size += sizeof(_y);

    return result_size;
}

const int cs_move::Serialize(OUT char* result)
{
    _size += (sizeof(cs_move) - sizeof(void*)) - (sizeof(BasePacket) - sizeof(void*));

    int result_size = 0;

    result_size += __super::Serialize(result);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_x, sizeof(_x));
    result_size += sizeof(_x);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_y, sizeof(_y));
    result_size += sizeof(_y);

    return result_size;
}

const int cs_move::Deserialize(const char* data)
{
    int result_size = 0;

    result_size += __super::Deserialize(data);

    memcpy_s(&_x, sizeof(_x), (void*)(data + result_size), sizeof(_x));
    result_size += sizeof(_x);

    memcpy_s(&_y, sizeof(_y), (void*)(data + result_size), sizeof(_y));
    result_size += sizeof(_y);

    return result_size;
}

const int sc_move::Serialize(OUT char* result)
{
    _size += (sizeof(sc_move) - sizeof(void*)) - (sizeof(BasePacket) - sizeof(void*));

    int result_size = 0;

    result_size += __super::Serialize(result);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_move_client_id, sizeof(_move_client_id));
    result_size += sizeof(_move_client_id);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_x, sizeof(_x));
    result_size += sizeof(_x);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_y, sizeof(_y));
    result_size += sizeof(_y);

    return result_size;
}

const int sc_move::Deserialize(const char* data)
{
    int result_size = 0;

    result_size += __super::Deserialize(data);

    memcpy_s(&_move_client_id, sizeof(_move_client_id), (void*)(data + result_size), sizeof(_move_client_id));
    result_size += sizeof(_move_client_id);

    memcpy_s(&_x, sizeof(_x), (void*)(data + result_size), sizeof(_x));
    result_size += sizeof(_x);

    memcpy_s(&_y, sizeof(_y), (void*)(data + result_size), sizeof(_y));
    result_size += sizeof(_y);

    return result_size;
}

const int cs_logout::Serialize(OUT char* result)
{
    _size += (sizeof(cs_logout) - sizeof(void*)) - (sizeof(BasePacket) - sizeof(void*));

    int result_size = 0;

    result_size += __super::Serialize(result);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_client_id, sizeof(_client_id));
    result_size += sizeof(_client_id);

    return result_size;
}

const int cs_logout::Deserialize(const char* data)
{
    int result_size = 0;

    result_size += __super::Deserialize(data);

    memcpy_s(&_client_id, sizeof(_client_id), (void*)(data + result_size), sizeof(_client_id));
    result_size += sizeof(_client_id);

    return result_size;
}

const int sc_logout::Serialize(OUT char* result)
{
    _size += (sizeof(sc_logout) - sizeof(void*)) - (sizeof(BasePacket) - sizeof(void*));

    int result_size = 0;

    result_size += __super::Serialize(result);

    memcpy_s(result + result_size, (rsize_t)result - result_size, (void*)&_client_id, sizeof(_client_id));
    result_size += sizeof(_client_id);

    return result_size;
}

const int sc_logout::Deserialize(const char* data)
{
    int result_size = 0;

    result_size += __super::Deserialize(data);

    memcpy_s(&_client_id, sizeof(_client_id), (void*)(data + result_size), sizeof(_client_id));
    result_size += sizeof(_client_id);

    return result_size;
}
