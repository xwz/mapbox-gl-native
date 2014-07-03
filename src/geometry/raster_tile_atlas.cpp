#include <llmr/geometry/raster_tile_atlas.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/util/image.hpp>

#include <cassert>

using namespace llmr;

RasterTileAtlas::RasterTileAtlas(uint16_t width, uint16_t height)
    : width(width),
      height(height),
      bin(width, height),
      data(new char[width * height]),
      dirty(true) {
}

RasterTileAtlas::~RasterTileAtlas() {
    delete[] data;
}

Rect<uint16_t> RasterTileAtlas::addTile(const std::string& source_url, const uint64_t tile_id, const Raster& raster) {
    std::lock_guard<std::mutex> lock(mtx);

    std::map<uint64_t, RasterTileValue>& source_tiles = index[source_url];
    std::map<uint64_t, RasterTileValue>::iterator it = source_tiles.find(tile_id);

    // The tile is already in this texture.
    if (it != source_tiles.end()) {
        RasterTileValue& value = it->second;
        return value.rect;
    }

    // The tile bitmap has zero width or height.
    if (!raster.width || !raster.height) {
        return Rect<uint16_t>{ 0, 0, 0, 0 };
    }

    uint16_t buffered_width = 256; //glyph.metrics.width + buffer * 2;
    uint16_t buffered_height = 256; //glyph.metrics.height + buffer * 2;

    // Add a 1px border around every image.
    uint16_t pack_width = buffered_width;
    uint16_t pack_height = buffered_height;

    // Increase to next number divisible by 4, but at least 1.
    // This is so we can scale down the texture coordinates and pack them
    // into 2 bytes rather than 4 bytes.
    pack_width += (4 - pack_width % 4);
    pack_height += (4 - pack_height % 4);

    Rect<uint16_t> rect = bin.allocate(pack_width, pack_height);
    if (rect.w == 0) {
        fprintf(stderr, "raster bitmap overflow");
        return rect;
    }

    assert(rect.x + rect.w <= width);
    assert(rect.y + rect.h <= height);

    source_tiles.emplace(tile_id, RasterTileValue { rect });

    // Copy the bitmap
    char *target = data;
    const char *source = raster.img->getData();
    for (uint32_t y = 0; y < buffered_height; y++) {
        uint32_t y1 = width * (rect.y + y) + rect.x;
        uint32_t y2 = buffered_width * y;
        for (uint32_t x = 0; x < buffered_width; x++) {
            target[y1 + x] = source[y2 + x];
        }
    }

    dirty = true;

    return rect;
}

void RasterTileAtlas::removeTile(const std::string& source_url, const uint64_t tile_id) {
    std::lock_guard<std::mutex> lock(mtx);

    std::map<uint64_t, RasterTileValue>& source_tiles = index[source_url];
    std::map<uint64_t, RasterTileValue>::iterator it = source_tiles.find(tile_id);

    if (it != source_tiles.end()) {
        RasterTileValue& value = it->second;

        const Rect<uint16_t>& rect = value.rect;

        // Clear out the bitmap.
        char *target = data;
        for (uint32_t y = 0; y < rect.h; y++) {
            uint32_t y1 = width * (rect.y + y) + rect.x;
            for (uint32_t x = 0; x < rect.w; x++) {
                target[y1 + x] = 0;
            }
        }

        dirty = true;

        bin.release(rect);

        source_tiles.erase(tile_id);
    }
}

void RasterTileAtlas::bind(Rect<uint16_t> rect) {
    if (rect) {
        if (!texture) {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } else {
            glBindTexture(GL_TEXTURE_2D, texture);
        }

        if (dirty) {
            std::lock_guard<std::mutex> lock(mtx);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, rect.w /*width*/, rect.h /*height*/, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
            dirty = false;
        }
    }
};
