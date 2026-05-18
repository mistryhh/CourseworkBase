#include "header.h"

#include "Psyhm9TileManager.h"

#include "ImageManager.h"

namespace
{
    constexpr int kMaskColour = 0x000000;

    std::string normaliseTheme(const std::string& theme)
    {
        if (theme == "grass" || theme == "sand" || theme == "dirt" || theme == "snow" || theme == "purple")
            return theme;
        return "grass";
    }

    SimpleImage loadTile(const std::string& path)
    {
        SimpleImage image = ImageManager::get()->getImagebyURL(path, true, true);
        image.setTransparencyColour(kMaskColour);
        return image;
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
    if (mapX < 0 || mapX >= getMapWidth() || mapY < 0)
        return true;
    if (mapY >= getMapHeight())
        return false;
    int value = getMapValue(mapX, mapY);
    return value == kPsyhm9TileSolid || value == kPsyhm9TileSpikes;
}

bool Psyhm9TileManager::isSolidAtPixel(int screenX, int screenY) const
{
    int mapX = getMapXForScreenX(screenX);
    int mapY = getMapYForScreenY(screenY);
    return isSolidTile(mapX, mapY);
}

bool Psyhm9TileManager::isHazardTile(int mapX, int mapY) const
{
    if (mapX < 0 || mapX >= getMapWidth() || mapY < 0 || mapY >= getMapHeight())
        return false;
    int value = getMapValue(mapX, mapY);
    return value == kPsyhm9TileSpikes || value == kPsyhm9TileLava;
}

void Psyhm9TileManager::virtDrawTileAt(
    BaseEngine* pEngine,
    DrawingSurface* pSurface,
    int iMapX, int iMapY,
    int iStartPositionScreenX, int iStartPositionScreenY) const
{
    int tileValue = getMapValue(iMapX, iMapY);
    if (tileValue == kPsyhm9TileEmpty)
        return;

    if (tileValue == kPsyhm9TileSpikes)
    {
        m_tileSet.spikes.renderImageBlit(
            pEngine,
            pSurface,
            iStartPositionScreenX, iStartPositionScreenY,
            getTileWidth(), getTileHeight(),
            0, 0,
            m_tileSet.spikes.getWidth(), m_tileSet.spikes.getHeight());
        return;
    }

    if (tileValue == kPsyhm9TileLava)
    {
        const SimpleImage& lavaFrame = ((pEngine->getRawTime() / kAnimatedTileFrameMs) % 2 == 0)
            ? m_tileSet.lavaTop
            : m_tileSet.lavaTopLow;
        lavaFrame.renderImageBlit(
            pEngine,
            pSurface,
            iStartPositionScreenX, iStartPositionScreenY,
            getTileWidth(), getTileHeight(),
            0, 0,
            lavaFrame.getWidth(), lavaFrame.getHeight());
        return;
    }

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

    tileImage->renderImageBlit(
        pEngine,
        pSurface,
        iStartPositionScreenX, iStartPositionScreenY,
        getTileWidth(), getTileHeight(),
        0, 0,
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
    m_tileSet.spikes = loadTile("resources/Tiles/spikes.png");
    m_tileSet.lavaTop = loadTile("resources/Tiles/lava_top.png");
    m_tileSet.lavaTopLow = loadTile("resources/Tiles/lava_top_low.png");
}
