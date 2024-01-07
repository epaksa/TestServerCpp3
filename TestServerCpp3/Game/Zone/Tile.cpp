#include "Tile.h"

const bool Tile::IsEmpty()
{
    if (_type != TileType::road)
    {
        return false;
    }

    if (_object != nullptr)
    {
        return false;
    }

    return true;
}
