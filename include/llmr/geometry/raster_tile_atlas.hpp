#ifndef LLMR_GEOMETRY_RASTER_TILE_ATLAS
#define LLMR_GEOMETRY_RASTER_TILE_ATLAS

#include <llmr/geometry/binpack.hpp>
#include <llmr/map/tile.hpp>
#include <llmr/util/raster.hpp>

#include <string>
#include <map>
#include <mutex>
#include <atomic>

namespace llmr {

    class RasterTileAtlas {
    public:

    private:
        struct RasterTileValue {
            RasterTileValue(const Rect<uint16_t>& rect)
                : rect(rect) {}
            Rect<uint16_t> rect;
        };

    public:
        RasterTileAtlas(uint16_t width, uint16_t height);
        ~RasterTileAtlas();

        Rect<uint16_t> addTile(const char *source_url, const Tile::ID& tile_id, const Raster& raster);
        void removeTile(const char *source_url, const Tile::ID& tile_id);
        void bind();

    public:
        const uint16_t width = 0;
        const uint16_t height = 0;

    private:
        std::mutex mtx;
        BinPack<uint16_t> bin;
        std::map<const char *, std::map<Tile::ID, RasterTileValue>> index;
        char *const data = nullptr;
        std::atomic<bool> dirty;
        uint32_t texture = 0;
    };

};

#endif
