#include "header.h"

#include "Psyhm9Game.h"

#include <cstdio>

Psyhm9Game::Psyhm9Game()
    : m_state(GameState::Menu)
    , m_tileManager()
    , m_level()
    , m_selectedLevel(0)
    , m_currentLevel(0)
    , m_player(nullptr)
    , m_goalX(0)
    , m_goalY(0)
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
    loadLevel(m_selectedLevel);
    return BaseEngine::virtInitialise();
}

void Psyhm9Game::loadAssets()
{
    m_menuBackground = loadImage("resources/Backgrounds/background_color_hills.png", true);
    m_backgroundGrass = loadImage("resources/Backgrounds/background_solid_grass.png", true);
    m_backgroundSand = loadImage("resources/Backgrounds/background_solid_sand.png", true);
    m_backgroundDirt = loadImage("resources/Backgrounds/background_solid_dirt.png", true);
    m_backgroundSnow = loadImage("resources/Backgrounds/background_solid_cloud.png", true);
    m_backgroundPurple = loadImage("resources/Backgrounds/background_color_mushrooms.png", true);
    m_flagImage = loadImage("resources/Tiles/flag_green_a.png", true);
    m_menuPlayerImage = loadImage("resources/Characters/character_green_front.png", true);
}

int Psyhm9Game::virtInitialiseObjects()
{
    drawableObjectsChanged();
    destroyOldObjects(true);

    createObjectArray(1);
    m_player = new Psyhm9Player(this, &m_tileManager);
    storeObjectInArray(0, m_player);
    m_player->setSpawnPosition(m_level.startTileX * kPsyhm9TileSize, m_level.startTileY * kPsyhm9TileSize);
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
    m_flagImage.renderImageWithMask(
        getBackgroundSurface(),
        0, 0,
        m_goalX, m_goalY,
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

    m_tileManager.drawAllTiles(this, getBackgroundSurface());

    int previewPlayerX = m_level.goalTileX * kPsyhm9TileSize - kPsyhm9TileSize;
    int previewPlayerY = m_level.goalTileY * kPsyhm9TileSize;
    m_menuPlayerImage.renderImageWithMask(
        getBackgroundSurface(),
        0, 0,
        previewPlayerX, previewPlayerY,
        m_menuPlayerImage.getWidth(), m_menuPlayerImage.getHeight());

    m_flagImage.renderImageWithMask(
        getBackgroundSurface(),
        0, 0,
        m_level.goalTileX * kPsyhm9TileSize, m_level.goalTileY * kPsyhm9TileSize,
        m_flagImage.getWidth(), m_flagImage.getHeight());
}

void Psyhm9Game::virtDrawStringsUnderneath()
{
    char buffer[128];
    switch (m_state)
    {
    case GameState::Menu:
        drawForegroundString(40, 30, "Kenney 2D Platformer - Psyhm9", 0xffffff, nullptr);
        drawForegroundString(40, 70, "Level complete preview below. Use LEFT/RIGHT to select level, ENTER to play.", 0xffffff, nullptr);
        drawSelectionRow(60, getWindowHeight() - 140);
        break;
    case GameState::Playing:
        std::snprintf(buffer, sizeof(buffer), "Level %d - P to pause, R to reload, M for menu", m_currentLevel + 1);
        drawForegroundString(40, 30, buffer, 0xffffff, nullptr);
        break;
    case GameState::Paused:
        drawForegroundString(40, 30, "Paused - Press P to resume or M for menu", 0xffffff, nullptr);
        break;
    case GameState::LevelComplete:
        std::snprintf(buffer, sizeof(buffer), "Level %d complete! ENTER for next level, M for menu.", m_currentLevel + 1);
        drawForegroundString(40, 30, buffer, 0x00ff99, nullptr);
        break;
    case GameState::GameComplete:
        drawForegroundString(40, 30, "All levels complete! ENTER for menu.", 0x00ff99, nullptr);
        break;
    }
}

void Psyhm9Game::virtDrawStringsOnTop()
{
    if (m_state == GameState::Menu)
        drawForegroundString(40, 110, "Use SPACE in game to jump, arrows/A-D to move.", 0xffffff, nullptr);
}

void Psyhm9Game::drawSelectionRow(int startX, int startY)
{
    for (int i = 0; i < static_cast<int>(m_levelFiles.size()); ++i)
    {
        int boxX = startX + i * 160;
        int boxY = startY;
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
        if (iKeyCode == SDLK_p)
        {
            setState(GameState::Paused);
        }
        else if (iKeyCode == SDLK_r)
        {
            startLevel(m_currentLevel);
        }
        else if (iKeyCode == SDLK_m)
        {
            setState(GameState::Menu);
        }
        else if (iKeyCode == SDLK_ESCAPE)
        {
            setExitWithCode(0);
        }
        break;
    case GameState::Paused:
        if (iKeyCode == SDLK_p)
        {
            setState(GameState::Playing);
        }
        else if (iKeyCode == SDLK_m)
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
        else if (iKeyCode == SDLK_m)
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
    if (m_state == GameState::Playing && playerReachedGoal())
        setState(GameState::LevelComplete);
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
        m_tileManager.loadFromLevel(m_level);
}

void Psyhm9Game::startLevel(int index)
{
    m_currentLevel = index;
    loadLevel(index);
    if (m_player)
        m_player->setSpawnPosition(m_level.startTileX * kPsyhm9TileSize, m_level.startTileY * kPsyhm9TileSize);
    setState(GameState::Playing);
}

void Psyhm9Game::setState(GameState state)
{
    m_state = state;
    if (m_state == GameState::Playing)
        unpause();
    else if (m_state == GameState::Paused || m_state == GameState::LevelComplete)
        pause();

    if (m_state == GameState::Menu)
        m_selectedLevel = m_currentLevel;

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
    int goalRight = m_goalX + m_flagImage.getWidth();
    int goalTop = m_goalY;
    int goalBottom = m_goalY + m_flagImage.getHeight();

    return !(playerRight < goalLeft || playerLeft > goalRight || playerBottom < goalTop || playerTop > goalBottom);
}
