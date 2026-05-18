#include "header.h"

#include "Psyhm9Player.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr double kMoveSpeed = 4.0;
    constexpr double kGravity = 0.8;
    constexpr double kJumpSpeed = -16.0;
    constexpr double kMaxFallSpeed = 18.0;
    constexpr int kMaskColour = 0x000000;

    void applyTransparency(SimpleImage& image)
    {
        image.setTransparencyColour(kMaskColour);
    }
}

Psyhm9Player::Psyhm9Player(BaseEngine* pEngine, Psyhm9TileManager* tileManager)
    : DisplayableObject(pEngine, 0, 0, true)
    , m_tileManager(tileManager)
    , m_idleImage(pEngine->loadImage("resources/Characters/character_green_front.png", true))
    , m_walkAImage(pEngine->loadImage("resources/Characters/character_green_walk_a.png", true))
    , m_walkBImage(pEngine->loadImage("resources/Characters/character_green_walk_b.png", true))
    , m_jumpImage(pEngine->loadImage("resources/Characters/character_green_jump.png", true))
    , m_posX(0.0)
    , m_posY(0.0)
    , m_velocityX(0.0)
    , m_velocityY(0.0)
    , m_onGround(false)
    , m_spawnX(0)
    , m_spawnY(0)
    , m_useWalkFrameA(true)
{
    applyTransparency(m_idleImage);
    applyTransparency(m_walkAImage);
    applyTransparency(m_walkBImage);
    applyTransparency(m_jumpImage);

    m_iDrawWidth = kPsyhm9TileSize;
    m_iDrawHeight = kPsyhm9TileSize;
    m_iStartDrawPosX = 0;
    m_iStartDrawPosY = 0;
    setVisible(true);
}

void Psyhm9Player::setSpawnPosition(int screenX, int screenY)
{
    m_spawnX = screenX;
    m_spawnY = screenY;
    resetForLevel();
}

void Psyhm9Player::resetForLevel()
{
    m_posX = static_cast<double>(m_spawnX);
    m_posY = static_cast<double>(m_spawnY);
    m_velocityX = 0.0;
    m_velocityY = 0.0;
    m_onGround = false;
    m_iCurrentScreenX = static_cast<int>(m_posX + 0.5);
    m_iCurrentScreenY = static_cast<int>(m_posY + 0.5);
    redrawDisplay();
}

void Psyhm9Player::virtDraw()
{
    if (!isVisible())
        return;

    const SimpleImage* frame = &m_idleImage;
    if (!m_onGround)
        frame = &m_jumpImage;
    else if (std::abs(m_velocityX) > 0.1)
        frame = m_useWalkFrameA ? &m_walkAImage : &m_walkBImage;

    frame->renderImageBlit(
        getEngine(),
        getEngine()->getForegroundSurface(),
        m_iCurrentScreenX + m_iStartDrawPosX,
        m_iCurrentScreenY + m_iStartDrawPosY,
        m_iDrawWidth, m_iDrawHeight,
        0, 0,
        frame->getWidth(), frame->getHeight());
}

void Psyhm9Player::virtDoUpdate(int iCurrentTime)
{
    (void)iCurrentTime;
    if (getEngine()->isPaused())
        return;

    bool movingLeft = getEngine()->isKeyPressed(SDLK_LEFT) || getEngine()->isKeyPressed(SDLK_a);
    bool movingRight = getEngine()->isKeyPressed(SDLK_RIGHT) || getEngine()->isKeyPressed(SDLK_d);
    bool jumpPressed = getEngine()->isKeyPressed(SDLK_SPACE) || getEngine()->isKeyPressed(SDLK_UP);

    if (movingLeft == movingRight)
        m_velocityX = 0.0;
    else if (movingLeft)
        m_velocityX = -kMoveSpeed;
    else
        m_velocityX = kMoveSpeed;

    if (jumpPressed && m_onGround)
    {
        m_velocityY = kJumpSpeed;
        m_onGround = false;
    }

    m_velocityY = std::min(m_velocityY + kGravity, kMaxFallSpeed);

    double nextX = m_posX + m_velocityX;
    resolveHorizontalMovement(nextX);

    double nextY = m_posY + m_velocityY;
    resolveVerticalMovement(nextY);

    updateAnimationFrame();

    m_iCurrentScreenX = static_cast<int>(m_posX + 0.5);
    m_iCurrentScreenY = static_cast<int>(m_posY + 0.5);
    redrawDisplay();
}

void Psyhm9Player::updateAnimationFrame()
{
    if (std::abs(m_velocityX) > 0.1 && m_onGround)
        m_useWalkFrameA = (getEngine()->getRawTime() / 150) % 2 == 0;
    else
        m_useWalkFrameA = true;
}

bool Psyhm9Player::resolveHorizontalMovement(double nextX)
{
    if (m_velocityX > 0.0)
    {
        int right = static_cast<int>(nextX) + m_iDrawWidth - 1;
        int top = static_cast<int>(m_posY);
        int bottom = static_cast<int>(m_posY) + m_iDrawHeight - 1;
        int mapX = m_tileManager->getMapXForScreenX(right);
        int startY = m_tileManager->getMapYForScreenY(top);
        int endY = m_tileManager->getMapYForScreenY(bottom);
        for (int mapY = startY; mapY <= endY; ++mapY)
        {
            if (m_tileManager->isSolidTile(mapX, mapY))
            {
                int tileLeft = m_tileManager->getBaseScreenX() + mapX * m_tileManager->getTileWidth();
                m_posX = static_cast<double>(tileLeft - m_iDrawWidth);
                m_velocityX = 0.0;
                return true;
            }
        }
    }
    else if (m_velocityX < 0.0)
    {
        int left = static_cast<int>(nextX);
        int top = static_cast<int>(m_posY);
        int bottom = static_cast<int>(m_posY) + m_iDrawHeight - 1;
        int mapX = m_tileManager->getMapXForScreenX(left);
        int startY = m_tileManager->getMapYForScreenY(top);
        int endY = m_tileManager->getMapYForScreenY(bottom);
        for (int mapY = startY; mapY <= endY; ++mapY)
        {
            if (m_tileManager->isSolidTile(mapX, mapY))
            {
                int tileRight = m_tileManager->getBaseScreenX() + (mapX + 1) * m_tileManager->getTileWidth();
                m_posX = static_cast<double>(tileRight);
                m_velocityX = 0.0;
                return true;
            }
        }
    }

    m_posX = nextX;
    return false;
}

bool Psyhm9Player::resolveVerticalMovement(double nextY)
{
    if (m_velocityY > 0.0)
    {
        int bottom = static_cast<int>(nextY) + m_iDrawHeight - 1;
        int left = static_cast<int>(m_posX);
        int right = static_cast<int>(m_posX) + m_iDrawWidth - 1;
        int mapY = m_tileManager->getMapYForScreenY(bottom);
        int startX = m_tileManager->getMapXForScreenX(left);
        int endX = m_tileManager->getMapXForScreenX(right);
        for (int mapX = startX; mapX <= endX; ++mapX)
        {
            if (m_tileManager->isSolidTile(mapX, mapY))
            {
                int tileTop = m_tileManager->getBaseScreenY() + mapY * m_tileManager->getTileHeight();
                m_posY = static_cast<double>(tileTop - m_iDrawHeight);
                m_velocityY = 0.0;
                m_onGround = true;
                return true;
            }
        }
        m_onGround = false;
    }
    else if (m_velocityY < 0.0)
    {
        int top = static_cast<int>(nextY);
        int left = static_cast<int>(m_posX);
        int right = static_cast<int>(m_posX) + m_iDrawWidth - 1;
        int mapY = m_tileManager->getMapYForScreenY(top);
        int startX = m_tileManager->getMapXForScreenX(left);
        int endX = m_tileManager->getMapXForScreenX(right);
        for (int mapX = startX; mapX <= endX; ++mapX)
        {
            if (m_tileManager->isSolidTile(mapX, mapY))
            {
                int tileBottom = m_tileManager->getBaseScreenY() + (mapY + 1) * m_tileManager->getTileHeight();
                m_posY = static_cast<double>(tileBottom);
                m_velocityY = 0.0;
                return true;
            }
        }
        m_onGround = false;
    }

    m_posY = nextY;
    return false;
}
