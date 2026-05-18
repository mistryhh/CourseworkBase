#include "header.h"

#include "Psyhm9Enemy.h"

namespace
{
    constexpr int kMaskColour = 0x000000;
    constexpr double kEnemySpeed = 2.0;

    void applyTransparency(SimpleImage& image)
    {
        image.setTransparencyColour(kMaskColour);
    }
}

Psyhm9Enemy::Psyhm9Enemy(BaseEngine* pEngine, Psyhm9TileManager* tileManager, int spawnTileX, int spawnTileY)
    : DisplayableObject(pEngine, 0, 0, true)
    , m_tileManager(tileManager)
    , m_walkAImage(pEngine->loadImage("resources/Enemies/slime_normal_walk_a.png", true))
    , m_walkBImage(pEngine->loadImage("resources/Enemies/slime_normal_walk_b.png", true))
    , m_posX(0.0)
    , m_posY(0.0)
    , m_velocityX(kEnemySpeed)
    , m_spawnTileX(spawnTileX)
    , m_spawnTileY(spawnTileY)
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

    double nextX = m_posX + m_velocityX;
    if (shouldReverse(nextX))
    {
        m_velocityX = -m_velocityX;
        nextX = m_posX + m_velocityX;
    }

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

bool Psyhm9Enemy::shouldReverse(double nextX) const
{
    int direction = m_velocityX >= 0.0 ? 1 : -1;
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
