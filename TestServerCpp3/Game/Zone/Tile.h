#pragma once

class Object;

enum class TileType : int
{
    road,
    wall
};

class Tile
{
public:
    const bool IsEmpty();

public:
    Object* _object = nullptr;
    TileType _type = TileType::road;
};

