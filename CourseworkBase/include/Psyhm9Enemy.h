#pragma once

#include "DisplayableObject.h"
#include "Psyhm9TileManager.h"
#include "SimpleImage.h"

class Psyhm9Player;

class Psyhm9Enemy : public DisplayableObject
{
public:
    Psyhm9Enemy(BaseEngine* pEngine, Psyhm9TileManager* tileManager, Psyhm9Player* player, int spawnTileX, int spawnTileY);

    void setSpawnTile(int tileX, int tileY);

    void virtDraw() override;
    void virtDoUpdate(int iCurrentTime) override;

private:
    void updateAnimationFrame();
    bool shouldReverse(double nextX, double velocityX) const;

    Psyhm9TileManager* m_tileManager;
    SimpleImage m_walkAImage;
    SimpleImage m_walkBImage;
    double m_posX;
    double m_posY;
    double m_velocityX;
    int m_spawnTileX;
    int m_spawnTileY;
    Psyhm9Player* m_player;
    bool m_useWalkFrameA;
};
