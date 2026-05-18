#pragma once

#include "DisplayableObject.h"
#include "Psyhm9TileManager.h"
#include "SimpleImage.h"

class Psyhm9Player : public DisplayableObject
{
public:
    Psyhm9Player(BaseEngine* pEngine, Psyhm9TileManager* tileManager);

    void setSpawnPosition(int screenX, int screenY);
    void resetForLevel();

    void virtDraw() override;
    void virtDoUpdate(int iCurrentTime) override;

private:
    void updateAnimationFrame();
    bool resolveHorizontalMovement(double nextX);
    bool resolveVerticalMovement(double nextY);

    Psyhm9TileManager* m_tileManager;
    SimpleImage m_idleImage;
    SimpleImage m_walkAImage;
    SimpleImage m_walkBImage;
    SimpleImage m_jumpImage;

    double m_posX;
    double m_posY;
    double m_velocityX;
    double m_velocityY;
    bool m_onGround;
    int m_spawnX;
    int m_spawnY;
    bool m_useWalkFrameA;
};
