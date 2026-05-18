#include "header.h"

#include "Psyhm9Level.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace
{
    std::string trim(const std::string& input)
    {
        size_t start = 0;
        while (start < input.size() && std::isspace(static_cast<unsigned char>(input[start])))
            ++start;
        size_t end = input.size();
        while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1])))
            --end;
        return input.substr(start, end - start);
    }
}

Psyhm9LevelData::Psyhm9LevelData()
    : theme("grass")
    , width(kPsyhm9MapWidth)
    , height(kPsyhm9MapHeight)
    , tiles(width * height, 0)
    , startTileX(0)
    , startTileY(0)
    , goalTileX(width - 1)
    , goalTileY(0)
    , enemies()
{
}

bool Psyhm9LevelData::loadFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return false;

    theme = "grass";
    width = kPsyhm9MapWidth;
    height = kPsyhm9MapHeight;
    tiles.assign(width * height, 0);
    enemies.clear();
    bool startSet = false;
    bool goalSet = false;
    std::vector<std::string> rows;

    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty())
            continue;
        if (line.rfind("theme=", 0) == 0)
        {
            theme = trim(line.substr(6));
            continue;
        }
        if (static_cast<int>(line.size()) >= width)
            rows.push_back(line.substr(0, width));
    }

    if (static_cast<int>(rows.size()) != height)
        return false;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            char value = rows[y][x];
            int tileValue = kPsyhm9TileEmpty;
            switch (value)
            {
            case '#':
                tileValue = kPsyhm9TileSolid;
                break;
            case '^':
                tileValue = kPsyhm9TileSpikes;
                break;
            case 'L':
                tileValue = kPsyhm9TileLava;
                break;
            case 'S':
                startTileX = x;
                startTileY = y;
                startSet = true;
                tileValue = kPsyhm9TileEmpty;
                break;
            case 'F':
                goalTileX = x;
                goalTileY = y;
                goalSet = true;
                tileValue = kPsyhm9TileEmpty;
                break;
            case 'E':
                enemies.push_back({x, y});
                tileValue = kPsyhm9TileEmpty;
                break;
            default:
                tileValue = kPsyhm9TileEmpty;
                break;
            }
            tiles[x + y * width] = tileValue;
        }
    }

    if (!startSet)
    {
        startTileX = 1;
        startTileY = height - 3;
    }

    if (!goalSet)
    {
        goalTileX = width - 2;
        goalTileY = height - 3;
    }

    return true;
}

int Psyhm9LevelData::getTile(int mapX, int mapY) const
{
    if (mapX < 0 || mapX >= width || mapY < 0 || mapY >= height)
        return 0;
    return tiles[mapX + mapY * width];
}
