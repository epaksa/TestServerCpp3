#include "Packet.h"
#include <corecrt_memcpy_s.h>

const int BasePacket::Serialize(OUT char* result)
{
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

const int sc_login::Deserialize(const char* data)
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
