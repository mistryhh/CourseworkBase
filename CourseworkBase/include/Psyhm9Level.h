#pragma once

#include <string>
#include <vector>

constexpr int kPsyhm9TileSize = 70;
constexpr int kPsyhm9MapWidth = 26;
constexpr int kPsyhm9MapHeight = 12;
constexpr int kPsyhm9TileEmpty = 0;
constexpr int kPsyhm9TileSolid = 1;
constexpr int kPsyhm9TileSpikes = 2;
constexpr int kPsyhm9TileLava = 3;

struct Psyhm9LevelData
{
    std::string theme;
    int width;
    int height;
    std::vector<int> tiles;
    int startTileX;
    int startTileY;
    int goalTileX;
    int goalTileY;
    struct EnemySpawn
    {
        int tileX;
        int tileY;
    };
    std::vector<EnemySpawn> enemies;

    Psyhm9LevelData();
    bool loadFromFile(const std::string& filePath);
    int getTile(int mapX, int mapY) const;
};
