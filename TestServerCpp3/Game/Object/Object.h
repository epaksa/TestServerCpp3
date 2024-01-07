#pragma once

enum class ObjectType : int
{
    player,
    monster
};

class Object
{
public:
    Object(const int id, const int hp);

public:
    int _id = 0;
    int _hp = 0;
};

