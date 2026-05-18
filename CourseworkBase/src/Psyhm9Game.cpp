#include "header.h"
#include "Psyhm9Game.h"
#include <algorithm>
#include <cstdio>

namespace
{
    constexpr int kMaskColour = 0x000000;
}

Psyhm9Game::Psyhm9Game()
    : m_state(GameState::Menu)
    , m_tileManager()
    , m_level()
    , m_selectedLevel(0)
    , m_currentLevel(0)
    , m_player(nullptr)
    , m_enemies()
    , m_goalX(0)
    , m_goalY(0)
    , m_gameOverStartTime(0)
    , m_cameraTranslation(0, 0, this)
    , m_cameraOffsetX(0)
    , m_cameraOffsetY(0)
    , m_lastAnimatedFrame(-1)
    , m_levelHasAnimatedTiles(false)
{
}

int Psyhm9Game::virtInitialise()
{
    m_levelFiles = {
        "resources/levels/Psyhm9_level1.txt",
        "resources/levels/Psyhm9_level2.txt",
        "resources/levels/Psyhm9_level3.txt",
        "resources/levels/Psyhm9_level4.txt",
        "resources/levels/Psyhm9_level5.txt"
    };
    loadAssets();
    getBackgroundSurface()->setDrawPointsFilter(&m_cameraTranslation);
    getForegroundSurface()->setDrawPointsFilter(&m_cameraTranslation);
    loadLevel(m_selectedLevel);
    return BaseEngine::virtInitialise();
}

void Psyhm9Game::loadAssets()
{
    m_menuBackground = loadImage("resources/Backgrounds/background_color_hills.png", true);
    m_backgroundGrass = loadImage("resources/Backgrounds/background_color_hills.png", true);
    m_backgroundSand = loadImage("resources/Backgrounds/background_color_desert.png", true);
    m_backgroundDirt = loadImage("resources/Backgrounds/background_color_trees.png", true);
    m_backgroundSnow = loadImage("resources/Backgrounds/background_solid_sky.png", true);
    m_backgroundPurple = loadImage("resources/Backgrounds/background_color_mushrooms.png", true);
    m_flagImage = loadImage("resources/Tiles/flag_green_a.png", true);
    m_menuPlayerImage = loadImage("resources/Characters/character_green_front.png", true);
    m_flagImage.setTransparencyColour(kMaskColour);
    m_menuPlayerImage.setTransparencyColour(kMaskColour);
}

int Psyhm9Game::virtInitialiseObjects()
{
    buildLevelObjects();
    return 0;
}

void Psyhm9Game::virtSetupBackgroundBuffer()
{
    switch (m_state)
    {
    case GameState::Menu:
        drawMenuBackground();
        break;
    case GameState::Playing:
    case GameState::Paused:
    case GameState::LevelComplete:
    case GameState::GameOver:
    case GameState::GameComplete:
        drawLevelBackground();
        break;
    }
}

void Psyhm9Game::drawLevelBackground()
{
    const SimpleImage& background = backgroundForTheme(m_level.theme);
    fillBackground(0x66aaff);

    int bgWidth = background.getWidth();
    int bgHeight = background.getHeight();
    for (int x = 0; x < getWindowWidth(); x += bgWidth)
        for (int y = 0; y < getWindowHeight(); y += bgHeight)
            background.renderImage(getBackgroundSurface(), 0, 0, x, y, bgWidth, bgHeight);

    m_tileManager.drawAllTiles(this, getBackgroundSurface());

    m_goalX = m_level.goalTileX * kPsyhm9TileSize;
    m_goalY = m_level.goalTileY * kPsyhm9TileSize;
    m_flagImage.renderImageBlit(
        this,
        getBackgroundSurface(),
        m_goalX, m_goalY,
        kPsyhm9TileSize, kPsyhm9TileSize,
        0, 0,
        m_flagImage.getWidth(), m_flagImage.getHeight());
}

void Psyhm9Game::drawMenuBackground()
{
    fillBackground(0x3355aa);
    int bgWidth = m_menuBackground.getWidth();
    int bgHeight = m_menuBackground.getHeight();
    for (int x = 0; x < getWindowWidth(); x += bgWidth)
        for (int y = 0; y < getWindowHeight(); y += bgHeight)
            m_menuBackground.renderImage(getBackgroundSurface(), 0, 0, x, y, bgWidth, bgHeight);
}

void Psyhm9Game::virtDrawStringsUnderneath()
{
    char buffer[128];
    switch (m_state)
    {
    case GameState::Menu:
        drawForegroundString(uiX(40), uiY(30), "Kenney 2D Platformer - Psyhm9", 0xffffff, nullptr);
        drawForegroundString(uiX(40), uiY(70), "Level complete preview below. Use LEFT/RIGHT to select level, ENTER to play.", 0xffffff, nullptr);
        drawSelectionRow(60, getWindowHeight() - 140);
        break;
    case GameState::Playing:
        std::snprintf(buffer, sizeof(buffer), "Level %d - P to pause, R to reload, M for menu", m_currentLevel + 1);
        drawForegroundString(uiX(40), uiY(30), buffer, 0xffffff, nullptr);
        break;
    case GameState::Paused:
        drawForegroundString(uiX(40), uiY(30), "Paused - Press P to resume or M for menu", 0xffffff, nullptr);
        break;
    case GameState::LevelComplete:
        std::snprintf(buffer, sizeof(buffer), "Level %d complete! ENTER for next level, M for menu.", m_currentLevel + 1);
        drawForegroundString(uiX(40), uiY(30), buffer, 0x00ff99, nullptr);
        break;
    case GameState::GameOver:
        drawForegroundString(uiX(40), uiY(30), "Game over! Restarting level...", 0xff6666, nullptr);
        break;
    case GameState::GameComplete:
        drawForegroundString(uiX(40), uiY(30), "All levels complete! ENTER for menu.", 0x00ff99, nullptr);
        break;
    }
}

void Psyhm9Game::virtDrawStringsOnTop()
{
    if (m_state == GameState::Menu)
        drawForegroundString(uiX(40), uiY(110), "Use SPACE in game to jump, arrows/A-D to move.", 0xffffff, nullptr);
}

void Psyhm9Game::drawSelectionRow(int startX, int startY)
{
    int baseX = uiX(startX);
    int baseY = uiY(startY);
    for (int i = 0; i < static_cast<int>(m_levelFiles.size()); ++i)
    {
        int boxX = baseX + i * 160;
        int boxY = baseY;
        int boxWidth = 130;
        int boxHeight = 60;
        if (i == m_selectedLevel)
            drawForegroundRectangle(boxX - 10, boxY - 10, boxX + boxWidth, boxY + boxHeight, 0xffcc33);
        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "Level %d", i + 1);
        drawForegroundString(boxX, boxY, buffer, 0xffffff, nullptr);
    }
}

void Psyhm9Game::virtKeyDown(int iKeyCode)
{
    switch (m_state)
    {
    case GameState::Menu:
        if (iKeyCode == SDLK_LEFT)
        {
            m_selectedLevel = (m_selectedLevel + static_cast<int>(m_levelFiles.size()) - 1) % static_cast<int>(m_levelFiles.size());
            loadLevel(m_selectedLevel);
            lockAndSetupBackground();
        }
        else if (iKeyCode == SDLK_RIGHT)
        {
            m_selectedLevel = (m_selectedLevel + 1) % static_cast<int>(m_levelFiles.size());
            loadLevel(m_selectedLevel);
            lockAndSetupBackground();
        }
        else if (iKeyCode == SDLK_RETURN || iKeyCode == SDLK_KP_ENTER)
        {
            startLevel(m_selectedLevel);
        }
        else if (iKeyCode == SDLK_ESCAPE)
        {
            setExitWithCode(0);
        }
        break;
    case GameState::Playing:
        if (iKeyCode == SDLK_P)
        {
            setState(GameState::Paused);
        }
        else if (iKeyCode == SDLK_R)
        {
            startLevel(m_currentLevel);
        }
        else if (iKeyCode == SDLK_M)
        {
            setState(GameState::Menu);
        }
        else if (iKeyCode == SDLK_ESCAPE)
        {
            setExitWithCode(0);
        }
        break;
    case GameState::Paused:
        if (iKeyCode == SDLK_P)
        {
            setState(GameState::Playing);
        }
        else if (iKeyCode == SDLK_M)
        {
            setState(GameState::Menu);
        }
        break;
    case GameState::LevelComplete:
        if (iKeyCode == SDLK_RETURN || iKeyCode == SDLK_KP_ENTER)
        {
            if (m_currentLevel + 1 < static_cast<int>(m_levelFiles.size()))
                startLevel(m_currentLevel + 1);
            else
                setState(GameState::GameComplete);
        }
        else if (iKeyCode == SDLK_M)
        {
            setState(GameState::Menu);
        }
        break;
    case GameState::GameOver:
        if (iKeyCode == SDLK_RETURN || iKeyCode == SDLK_KP_ENTER || iKeyCode == SDLK_R)
        {
            startLevel(m_currentLevel);
        }
        else if (iKeyCode == SDLK_M)
        {
            setState(GameState::Menu);
        }
        break;
    case GameState::GameComplete:
        if (iKeyCode == SDLK_RETURN || iKeyCode == SDLK_KP_ENTER)
            setState(GameState::Menu);
        break;
    }
}

void Psyhm9Game::virtMainLoopPostUpdate()
{
    if (m_state == GameState::Playing)
    {
        if (playerReachedGoal())
        {
            setState(GameState::LevelComplete);
            return;
        }
        if (playerHitEnemy() || playerHitHazard() || playerFellOut())
        {
            m_gameOverStartTime = getRawTime();
            setState(GameState::GameOver);
            return;
        }
        updateCamera(false);
    }

    if (m_state == GameState::GameOver)
    {
        if (getRawTime() - m_gameOverStartTime > 1400)
            startLevel(m_currentLevel);
    }
}

void Psyhm9Game::drawAllObjects()
{
    if (m_state == GameState::Playing)
        BaseEngine::drawAllObjects();
}

void Psyhm9Game::updateAllObjects(int iCurrentTime)
{
    if (m_state == GameState::Playing)
        BaseEngine::updateAllObjects(iCurrentTime);
}

void Psyhm9Game::loadLevel(int index)
{
    if (index < 0 || index >= static_cast<int>(m_levelFiles.size()))
        return;

    if (m_level.loadFromFile(m_levelFiles[index]))
    {
        m_tileManager.loadFromLevel(m_level);
        m_levelHasAnimatedTiles = std::any_of(
            m_level.tiles.begin(),
            m_level.tiles.end(),
            [](int value) { return value == kPsyhm9TileLava; });
    }
}

void Psyhm9Game::buildLevelObjects()
{
    drawableObjectsChanged();
    destroyOldObjects(true);
    m_enemies.clear();

    int enemyCount = static_cast<int>(m_level.enemies.size());
    createObjectArray(1 + enemyCount);
    m_player = new Psyhm9Player(this, &m_tileManager);
    storeObjectInArray(0, m_player);
    m_player->setSpawnPosition(m_level.startTileX * kPsyhm9TileSize, m_level.startTileY * kPsyhm9TileSize);

    for (int i = 0; i < enemyCount; ++i)
    {
        const auto& enemy = m_level.enemies[i];
        Psyhm9Enemy* enemyObject = new Psyhm9Enemy(this, &m_tileManager, m_player, enemy.tileX, enemy.tileY);
        storeObjectInArray(i + 1, enemyObject);
        m_enemies.push_back(enemyObject);
    }
}

void Psyhm9Game::startLevel(int index)
{
    m_currentLevel = index;
    loadLevel(index);
    buildLevelObjects();
    m_lastAnimatedFrame = -1;
    updateCameraOffsets();
    setState(GameState::Playing);
}

void Psyhm9Game::setState(GameState state)
{
    m_state = state;
    if (m_state == GameState::Playing)
        unpause();
    else if (m_state == GameState::Paused || m_state == GameState::LevelComplete || m_state == GameState::GameOver)
        pause();

    if (m_state == GameState::Menu)
    {
        m_selectedLevel = m_currentLevel;
        setCameraOffset(0, 0);
        m_lastAnimatedFrame = -1;
    }

    lockAndSetupBackground();
    redrawDisplay();
}

const SimpleImage& Psyhm9Game::backgroundForTheme(const std::string& theme) const
{
    if (theme == "sand")
        return m_backgroundSand;
    if (theme == "dirt")
        return m_backgroundDirt;
    if (theme == "snow")
        return m_backgroundSnow;
    if (theme == "purple")
        return m_backgroundPurple;
    return m_backgroundGrass;
}

bool Psyhm9Game::playerReachedGoal() const
{
    if (!m_player)
        return false;

    int playerLeft = m_player->getDrawingRegionLeft();
    int playerRight = m_player->getDrawingRegionRight();
    int playerTop = m_player->getDrawingRegionTop();
    int playerBottom = m_player->getDrawingRegionBottom();

    int goalLeft = m_goalX;
    int goalRight = m_goalX + kPsyhm9TileSize;
    int goalTop = m_goalY;
    int goalBottom = m_goalY + kPsyhm9TileSize;

    return !(playerRight < goalLeft || playerLeft > goalRight || playerBottom < goalTop || playerTop > goalBottom);
}

bool Psyhm9Game::playerHitEnemy() const
{
    if (!m_player)
        return false;

    int playerLeft = m_player->getDrawingRegionLeft();
    int playerRight = m_player->getDrawingRegionRight();
    int playerTop = m_player->getDrawingRegionTop();
    int playerBottom = m_player->getDrawingRegionBottom();

    for (const auto* enemy : m_enemies)
    {
        if (!enemy)
            continue;

        int enemyLeft = enemy->getDrawingRegionLeft();
        int enemyRight = enemy->getDrawingRegionRight();
        int enemyTop = enemy->getDrawingRegionTop();
        int enemyBottom = enemy->getDrawingRegionBottom();

        if (!(playerRight < enemyLeft || playerLeft > enemyRight || playerBottom < enemyTop || playerTop > enemyBottom))
            return true;
    }

    return false;
}

bool Psyhm9Game::playerFellOut() const
{
    if (!m_player)
        return false;

    return m_player->getDrawingRegionTop() > getWindowHeight();
}

bool Psyhm9Game::playerHitHazard() const
{
    if (!m_player)
        return false;

    int left = m_player->getDrawingRegionLeft();
    int right = m_player->getDrawingRegionRight();
    int top = m_player->getDrawingRegionTop();
    int bottom = m_player->getDrawingRegionBottom();

    int startX = m_tileManager.getMapXForScreenX(left);
    int endX = m_tileManager.getMapXForScreenX(right);
    int startY = m_tileManager.getMapYForScreenY(top);
    int endY = m_tileManager.getMapYForScreenY(bottom);

    for (int mapY = startY; mapY <= endY; ++mapY)
    {
        for (int mapX = startX; mapX <= endX; ++mapX)
        {
            if (m_tileManager.isHazardTile(mapX, mapY))
                return true;
        }
    }

    return false;
}

void Psyhm9Game::updateCamera(bool forceRedraw)
{
    if (!m_player)
        return;

    int previousOffsetX = m_cameraOffsetX;
    int previousOffsetY = m_cameraOffsetY;
    updateCameraOffsets();
    bool offsetChanged = previousOffsetX != m_cameraOffsetX || previousOffsetY != m_cameraOffsetY;

    int animationFrame = 0;
    bool animationChanged = false;
    if (m_levelHasAnimatedTiles)
    {
        animationFrame = (getRawTime() / Psyhm9TileManager::kAnimatedTileFrameMs) % 2;
        animationChanged = animationFrame != m_lastAnimatedFrame;
    }

    if (forceRedraw || offsetChanged || animationChanged)
    {
        if (m_levelHasAnimatedTiles)
            m_lastAnimatedFrame = animationFrame;
        lockAndSetupBackground();
        redrawDisplay();
    }
    else if (m_levelHasAnimatedTiles)
    {
        m_lastAnimatedFrame = animationFrame;
    }
}

void Psyhm9Game::updateCameraOffsets()
{
    if (!m_player)
        return;

    int playerCenterX = (m_player->getDrawingRegionLeft() + m_player->getDrawingRegionRight()) / 2;
    int playerCenterY = (m_player->getDrawingRegionTop() + m_player->getDrawingRegionBottom()) / 2;
    int levelWidth = m_level.width * kPsyhm9TileSize;
    int levelHeight = m_level.height * kPsyhm9TileSize;

    int minOffsetX = std::min(0, getWindowWidth() - levelWidth);
    int minOffsetY = std::min(0, getWindowHeight() - levelHeight);
    int newOffsetX = std::clamp(getWindowWidth() / 2 - playerCenterX, minOffsetX, 0);
    int newOffsetY = std::clamp(getWindowHeight() / 2 - playerCenterY, minOffsetY, 0);

    setCameraOffset(newOffsetX, newOffsetY);
}

void Psyhm9Game::setCameraOffset(int offsetX, int offsetY)
{
    m_cameraOffsetX = offsetX;
    m_cameraOffsetY = offsetY;
    m_cameraTranslation.setOffset(offsetX, offsetY);
}

int Psyhm9Game::uiX(int screenX) const
{
    return screenX - m_cameraOffsetX;
}

int Psyhm9Game::uiY(int screenY) const
{
    return screenY - m_cameraOffsetY;
}
