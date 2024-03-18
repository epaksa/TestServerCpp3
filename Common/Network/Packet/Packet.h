#pragma once
#include <vector>

#define OUT

#pragma pack(push, 1)

enum class PacketID : int
{
    none,
    cs_login,
    sc_login,
    sc_welcome,
    cs_move,
    sc_move,
    cs_logout,
    sc_logout
};

struct BasePacket
{
    int _size = 0;
    PacketID _packet_id = PacketID::none;

    BasePacket(const PacketID packet_id) : _packet_id(packet_id)
    {

    }

    virtual const int Serialize(OUT char* result);
    virtual const int Deserialize(const char* data);
};

struct cs_login : public BasePacket
{
    int _client_id = 0;

    cs_login() : BasePacket(PacketID::cs_login)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

struct sc_login : public BasePacket
{
    struct ClientPos
    {
        int _client_id = 0;
        int _x = 0;
        int _y = 0;
    };

    ClientPos _my_info;
    std::vector<ClientPos> _list_client;

    sc_login() : BasePacket(PacketID::sc_login)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

struct sc_welcome : public BasePacket
{
    int _client_id = 0;
    int _x = 0;
    int _y = 0;

    sc_welcome() : BasePacket(PacketID::sc_welcome)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

struct cs_move : public BasePacket
{
    int _x = 0;
    int _y = 0;

    cs_move() : BasePacket(PacketID::cs_move)
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

    sc_move() : BasePacket(PacketID::sc_move)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

struct cs_logout : public BasePacket
{
    int _client_id = 0;

    cs_logout() : BasePacket(PacketID::cs_logout)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

struct sc_logout : public BasePacket
{
    int _client_id = 0;

    sc_logout() : BasePacket(PacketID::sc_logout)
    {

    }

    virtual const int Serialize(OUT char* result) final override;
    virtual const int Deserialize(const char* data) final override;
};

#pragma pack(pop)
