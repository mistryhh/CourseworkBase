#include "header.h"

#include "Psyhm9TileManager.h"

#include "ImageManager.h"

namespace
{
    std::string normaliseTheme(const std::string& theme)
    {
        if (theme == "grass" || theme == "sand" || theme == "dirt" || theme == "snow" || theme == "purple")
            return theme;
        return "grass";
    }

    SimpleImage loadTile(const std::string& path)
    {
        return ImageManager::get()->getImagebyURL(path, true, true);
    }
}

Psyhm9TileManager::Psyhm9TileManager()
    : TileManager(kPsyhm9TileSize, kPsyhm9TileSize)
    , m_theme("grass")
{
    loadTileSet(m_theme);
}

void Psyhm9TileManager::setTheme(const std::string& theme)
{
    std::string normalised = normaliseTheme(theme);
    if (normalised == m_theme)
        return;
    m_theme = normalised;
    loadTileSet(m_theme);
}

void Psyhm9TileManager::loadFromLevel(const Psyhm9LevelData& level)
{
    setTheme(level.theme);
    setMapSize(level.width, level.height);
    for (int y = 0; y < level.height; ++y)
    {
        for (int x = 0; x < level.width; ++x)
            setMapValue(x, y, level.getTile(x, y));
    }
    setTopLeftPositionOnScreen(0, 0);
}

bool Psyhm9TileManager::isSolidTile(int mapX, int mapY) const
{
    if (mapX < 0 || mapY < 0 || mapX >= getMapWidth() || mapY >= getMapHeight())
        return true;
    return getMapValue(mapX, mapY) != 0;
}

bool Psyhm9TileManager::isSolidAtPixel(int screenX, int screenY) const
{
    int mapX = getMapXForScreenX(screenX);
    int mapY = getMapYForScreenY(screenY);
    return isSolidTile(mapX, mapY);
}

void Psyhm9TileManager::virtDrawTileAt(
    BaseEngine* pEngine,
    DrawingSurface* pSurface,
    int iMapX, int iMapY,
    int iStartPositionScreenX, int iStartPositionScreenY) const
{
    (void)pEngine;
    if (getMapValue(iMapX, iMapY) == 0)
        return;

    bool solidAbove = isSolidTile(iMapX, iMapY - 1);
    bool solidBelow = isSolidTile(iMapX, iMapY + 1);
    bool solidLeft = isSolidTile(iMapX - 1, iMapY);
    bool solidRight = isSolidTile(iMapX + 1, iMapY);

    const SimpleImage* tileImage = &m_tileSet.blockCenter;
    if (!solidAbove)
    {
        if (!solidLeft)
            tileImage = &m_tileSet.blockTopLeft;
        else if (!solidRight)
            tileImage = &m_tileSet.blockTopRight;
        else
            tileImage = &m_tileSet.blockTop;
    }
    else if (!solidBelow)
    {
        if (!solidLeft)
            tileImage = &m_tileSet.blockBottomLeft;
        else if (!solidRight)
            tileImage = &m_tileSet.blockBottomRight;
        else
            tileImage = &m_tileSet.blockBottom;
    }
    else if (!solidLeft)
    {
        tileImage = &m_tileSet.blockLeft;
    }
    else if (!solidRight)
    {
        tileImage = &m_tileSet.blockRight;
    }

    tileImage->renderImageWithMask(
        pSurface,
        0, 0,
        iStartPositionScreenX, iStartPositionScreenY,
        tileImage->getWidth(), tileImage->getHeight());
}

void Psyhm9TileManager::loadTileSet(const std::string& theme)
{
    const std::string prefix = "resources/Tiles/terrain_" + theme + "_";
    m_tileSet.blockTop = loadTile(prefix + "block_top.png");
    m_tileSet.blockTopLeft = loadTile(prefix + "block_top_left.png");
    m_tileSet.blockTopRight = loadTile(prefix + "block_top_right.png");
    m_tileSet.blockBottom = loadTile(prefix + "block_bottom.png");
    m_tileSet.blockBottomLeft = loadTile(prefix + "block_bottom_left.png");
    m_tileSet.blockBottomRight = loadTile(prefix + "block_bottom_right.png");
    m_tileSet.blockLeft = loadTile(prefix + "block_left.png");
    m_tileSet.blockRight = loadTile(prefix + "block_right.png");
    m_tileSet.blockCenter = loadTile(prefix + "block_center.png");
}
