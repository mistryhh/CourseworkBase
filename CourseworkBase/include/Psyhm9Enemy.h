#pragma once

#include "DisplayableObject.h"
#include "Psyhm9TileManager.h"
#include "SimpleImage.h"

class Psyhm9Enemy : public DisplayableObject
{
public:
    Psyhm9Enemy(BaseEngine* pEngine, Psyhm9TileManager* tileManager, int spawnTileX, int spawnTileY);

    void setSpawnTile(int tileX, int tileY);

    void virtDraw() override;
    void virtDoUpdate(int iCurrentTime) override;

private:
    void updateAnimationFrame();
    bool shouldReverse(double nextX) const;

    Psyhm9TileManager* m_tileManager;
    SimpleImage m_walkAImage;
    SimpleImage m_walkBImage;
    double m_posX;
    double m_posY;
    double m_velocityX;
    int m_spawnTileX;
    int m_spawnTileY;
    bool m_useWalkFrameA;
};
