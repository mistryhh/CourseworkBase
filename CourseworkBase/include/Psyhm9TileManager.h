#pragma once

#include "Psyhm9Level.h"
#include "SimpleImage.h"
#include "TileManager.h"

#include <string>

class Psyhm9TileManager : public TileManager
{
public:
    static constexpr int kAnimatedTileFrameMs = 250;

    Psyhm9TileManager();

    void setTheme(const std::string& theme);
    void loadFromLevel(const Psyhm9LevelData& level);
    bool isSolidTile(int mapX, int mapY) const;
    bool isSolidAtPixel(int screenX, int screenY) const;
    bool isHazardTile(int mapX, int mapY) const;
    int getBaseScreenX() const { return m_iBaseScreenX; }
    int getBaseScreenY() const { return m_iBaseScreenY; }

    void virtDrawTileAt(
        BaseEngine* pEngine,
        DrawingSurface* pSurface,
        int iMapX, int iMapY,
        int iStartPositionScreenX, int iStartPositionScreenY) const override;

private:
    struct TileSet
    {
        SimpleImage blockTop;
        SimpleImage blockTopLeft;
        SimpleImage blockTopRight;
        SimpleImage blockBottom;
        SimpleImage blockBottomLeft;
        SimpleImage blockBottomRight;
        SimpleImage blockLeft;
        SimpleImage blockRight;
        SimpleImage blockCenter;
        SimpleImage spikes;
        SimpleImage lavaTop;
        SimpleImage lavaTopLow;
    };

    void loadTileSet(const std::string& theme);

    TileSet m_tileSet;
    std::string m_theme;
};
