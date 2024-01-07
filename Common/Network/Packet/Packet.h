#pragma once

#define OUT

#pragma pack(push, 1)

enum class PacketID : int
{
    none,
    cs_login,
    sc_login,
    cs_move,
    sc_move,
    sc_logout
};

struct BasePacket
{
    int _size = 0;
    PacketID _packet_id = PacketID::none;

    BasePacket(const int size, const PacketID packet_id) : _size(size), _packet_id(packet_id)
    {

    }

    virtual const int Serialize(OUT char* result);
    virtual const int Deserialize(const char* data);
};

struct cs_login : public BasePacket
{
    int _client_id = 0;

    cs_login() : BasePacket(sizeof(cs_login) - sizeof(void*), PacketID::cs_login)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

struct sc_login : public BasePacket
{
    int _client_id = 0;
    int _x = 0;
    int _y = 0;

    sc_login() : BasePacket(sizeof(sc_login) - sizeof(void*), PacketID::sc_login)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

struct cs_move : public BasePacket
{
    int _x = 0;
    int _y = 0;

    cs_move() : BasePacket(sizeof(cs_move) - sizeof(void*), PacketID::cs_move)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

struct sc_move : public BasePacket
{
    int _move_client_id = 0;
    int _x = 0;
    int _y = 0;

    sc_move() : BasePacket(sizeof(sc_move) - sizeof(void*), PacketID::sc_move)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

#pragma pack(pop)
