#pragma once

#include "BaseEngine.h"
#include "Psyhm9Enemy.h"
#include "Psyhm9Level.h"
#include "Psyhm9Player.h"
#include "Psyhm9TileManager.h"
#include "ExampleFilterPointClasses.h"

#include <string>
#include <vector>

class Psyhm9Game : public BaseEngine
{
public:
    Psyhm9Game();
    ~Psyhm9Game() override = default;

    int virtInitialise() override;
    void virtSetupBackgroundBuffer() override;
    int virtInitialiseObjects() override;
    void virtDrawStringsUnderneath() override;
    void virtDrawStringsOnTop() override;
    void virtKeyDown(int iKeyCode) override;
    void virtMainLoopPostUpdate() override;
    void drawAllObjects() override;
    void updateAllObjects(int iCurrentTime) override;

private:
    enum class GameState
    {
        Menu,
        Playing,
        Paused,
        LevelComplete,
        GameOver,
        GameComplete
    };

    void loadAssets();
    void loadLevel(int index);
    void buildLevelObjects();
    void startLevel(int index);
    void drawLevelBackground();
    void drawMenuBackground();
    void drawSelectionRow(int startX, int startY);
    void setState(GameState state);
    const SimpleImage& backgroundForTheme(const std::string& theme) const;
    bool playerReachedGoal() const;
    bool playerHitEnemy() const;
    bool playerHitHazard() const;
    bool playerFellOut() const;
    void updateCamera(bool forceRedraw);
    void updateCameraOffsets();
    void setCameraOffset(int offsetX, int offsetY);
    int uiX(int screenX) const;
    int uiY(int screenY) const;

    GameState m_state;
    Psyhm9TileManager m_tileManager;
    Psyhm9LevelData m_level;
    std::vector<std::string> m_levelFiles;
    int m_selectedLevel;
    int m_currentLevel;
    Psyhm9Player* m_player;
    std::vector<Psyhm9Enemy*> m_enemies;
    SimpleImage m_menuBackground;
    SimpleImage m_backgroundGrass;
    SimpleImage m_backgroundSand;
    SimpleImage m_backgroundDirt;
    SimpleImage m_backgroundSnow;
    SimpleImage m_backgroundPurple;
    SimpleImage m_flagImage;
    SimpleImage m_menuPlayerImage;
    int m_goalX;
    int m_goalY;
    int m_gameOverStartTime;
    FilterPointsTranslation m_cameraTranslation;
    int m_cameraOffsetX;
    int m_cameraOffsetY;
    int m_lastAnimatedFrame;
    bool m_levelHasAnimatedTiles;
};
