#include "header.h"

#include "Psyhm9Enemy.h"
#include "Psyhm9Player.h"

#include <cmath>

namespace
{
    constexpr int kMaskColour = 0x000000;
    constexpr double kEnemySpeed = 2.0;
    constexpr double kChaseSpeed = 3.0;
    constexpr int kChaseRangeTiles = 6;
    constexpr int kChaseVerticalTiles = 2;

    void applyTransparency(SimpleImage& image)
    {
        image.setTransparencyColour(kMaskColour);
    }
}

Psyhm9Enemy::Psyhm9Enemy(BaseEngine* pEngine, Psyhm9TileManager* tileManager, Psyhm9Player* player, int spawnTileX, int spawnTileY)
    : DisplayableObject(pEngine, 0, 0, true)
    , m_tileManager(tileManager)
    , m_walkAImage(pEngine->loadImage("resources/Enemies/slime_normal_walk_a.png", true))
    , m_walkBImage(pEngine->loadImage("resources/Enemies/slime_normal_walk_b.png", true))
    , m_posX(0.0)
    , m_posY(0.0)
    , m_velocityX(kEnemySpeed)
    , m_spawnTileX(spawnTileX)
    , m_spawnTileY(spawnTileY)
    , m_player(player)
    , m_useWalkFrameA(true)
{
    applyTransparency(m_walkAImage);
    applyTransparency(m_walkBImage);

    m_iDrawWidth = kPsyhm9TileSize;
    m_iDrawHeight = kPsyhm9TileSize;
    m_iStartDrawPosX = 0;
    m_iStartDrawPosY = 0;
    setVisible(true);

    setSpawnTile(spawnTileX, spawnTileY);
}

void Psyhm9Enemy::setSpawnTile(int tileX, int tileY)
{
    m_spawnTileX = tileX;
    m_spawnTileY = tileY;
    m_posX = static_cast<double>(tileX * kPsyhm9TileSize);
    m_posY = static_cast<double>(tileY * kPsyhm9TileSize);
    m_velocityX = kEnemySpeed;
    m_useWalkFrameA = true;
    m_iCurrentScreenX = static_cast<int>(m_posX + 0.5);
    m_iCurrentScreenY = static_cast<int>(m_posY + 0.5);
    redrawDisplay();
}

void Psyhm9Enemy::virtDraw()
{
    if (!isVisible())
        return;

    const SimpleImage& frame = m_useWalkFrameA ? m_walkAImage : m_walkBImage;
    frame.renderImageBlit(
        getEngine(),
        getEngine()->getForegroundSurface(),
        m_iCurrentScreenX + m_iStartDrawPosX,
        m_iCurrentScreenY + m_iStartDrawPosY,
        m_iDrawWidth, m_iDrawHeight,
        0, 0,
        frame.getWidth(), frame.getHeight());
}

void Psyhm9Enemy::virtDoUpdate(int iCurrentTime)
{
    (void)iCurrentTime;
    if (getEngine()->isPaused())
        return;

    double desiredVelocity = m_velocityX;
    bool chasing = false;
    if (m_player)
    {
        int playerCenterX = (m_player->getDrawingRegionLeft() + m_player->getDrawingRegionRight()) / 2;
        int playerCenterY = (m_player->getDrawingRegionTop() + m_player->getDrawingRegionBottom()) / 2;
        int enemyCenterX = static_cast<int>(m_posX) + m_iDrawWidth / 2;
        int enemyCenterY = static_cast<int>(m_posY) + m_iDrawHeight / 2;
        int dx = playerCenterX - enemyCenterX;
        int dy = playerCenterY - enemyCenterY;
        if (std::abs(dx) <= kChaseRangeTiles * kPsyhm9TileSize
            && std::abs(dy) <= kChaseVerticalTiles * kPsyhm9TileSize)
        {
            chasing = true;
            desiredVelocity = (dx < 0) ? -kChaseSpeed : kChaseSpeed;
        }
    }

    double nextX = m_posX + desiredVelocity;
    if (desiredVelocity != 0.0)
    {
        if (shouldReverse(nextX, desiredVelocity))
        {
            if (chasing)
            {
                desiredVelocity = 0.0;
                nextX = m_posX;
            }
            else
            {
                desiredVelocity = -desiredVelocity;
                nextX = m_posX + desiredVelocity;
            }
        }
    }

    m_velocityX = desiredVelocity;
    m_posX = nextX;
    updateAnimationFrame();

    m_iCurrentScreenX = static_cast<int>(m_posX + 0.5);
    m_iCurrentScreenY = static_cast<int>(m_posY + 0.5);
    redrawDisplay();
}

void Psyhm9Enemy::updateAnimationFrame()
{
    m_useWalkFrameA = (getEngine()->getRawTime() / 200) % 2 == 0;
}

bool Psyhm9Enemy::shouldReverse(double nextX, double velocityX) const
{
    int direction = velocityX >= 0.0 ? 1 : -1;
    int frontX = direction > 0 ? static_cast<int>(nextX) + m_iDrawWidth - 1 : static_cast<int>(nextX);
    int top = static_cast<int>(m_posY);
    int bottom = static_cast<int>(m_posY) + m_iDrawHeight - 1;
    int mapX = m_tileManager->getMapXForScreenX(frontX);
    int startY = m_tileManager->getMapYForScreenY(top);
    int endY = m_tileManager->getMapYForScreenY(bottom);
    for (int mapY = startY; mapY <= endY; ++mapY)
    {
        if (m_tileManager->isSolidTile(mapX, mapY))
            return true;
    }

    int footY = static_cast<int>(m_posY) + m_iDrawHeight;
    if (!m_tileManager->isSolidAtPixel(frontX, footY + 1))
        return true;

    return false;
}
