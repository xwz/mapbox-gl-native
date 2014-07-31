#ifndef LLMR_GEOMETRY_RASTER_TILE_ATLAS
#define LLMR_GEOMETRY_RASTER_TILE_ATLAS

#include <mbgl/geometry/binpack.hpp>
#include <mbgl/util/raster.hpp>

#include <string>
#include <map>
#include <mutex>
#include <atomic>

namespace mbgl {

class RasterTileAtlas {
    private:
        struct RasterTileValue {
            RasterTileValue(const Rect<uint16_t>& rect)
                : rect(rect) {}
            Rect<uint16_t> rect;
        };

    public:
        RasterTileAtlas(uint16_t width, uint16_t height);
        ~RasterTileAtlas();

    Rect<uint16_t> addTile(const std::string& source_url, const uint64_t tile_id, const std::shared_ptr<Raster> raster);
        void removeTile(const std::string& source_url, const uint64_t tile_id);
        void bind(Rect<uint16_t> rect);

        inline float getWidth() const { return width; }
        inline float getHeight() const { return height; }

    public:
        const uint16_t width = 0;
        const uint16_t height = 0;

    private:
        std::mutex mtx;
        BinPack<uint16_t> bin;
        std::map<std::string, std::map<uint64_t, RasterTileValue>> index;
        char *const data = nullptr;
        std::atomic<bool> dirty;
        uint32_t texture = 0;
    };

};

#endif
