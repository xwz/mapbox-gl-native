#include <mbgl/geometry/raster_tile_atlas.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/string.hpp>

#include <cassert>

using namespace mbgl;

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

void raster_copy_bitmap(const uint32_t *src, const int src_stride, const int src_x, const int src_y,
                 uint32_t *dst, const int dst_stride, const int dst_x, const int dst_y,
                 const int width, const int height) {
    src += src_y * src_stride + src_x;
    dst += dst_y * dst_stride + dst_x;
    for (int y = 0; y < height; y++, src += src_stride, dst += dst_stride) {
        for (int x = 0; x < width; x++) {
            const uint8_t *s = reinterpret_cast<const uint8_t *>(src + x);
            uint8_t *d = reinterpret_cast<uint8_t *>(dst + x);

            // Premultiply the bitmap.
            // Note: We don't need to clamp the component values to 0..255, since
            // the source value is already 0..255 and the operation means they will
            // stay within the range of 0..255 and won't overflow.
            const uint8_t a = s[3];
            d[0] = s[0] * a / 255;
            d[1] = s[1] * a / 255;
            d[2] = s[2] * a / 255;
            d[3] = a;
        }
    }
}

Rect<uint16_t> RasterTileAtlas::addTile(const std::string& source_url, const uint64_t tile_id, const std::shared_ptr<Raster> raster) {
    std::lock_guard<std::mutex> lock(mtx);

    std::map<uint64_t, RasterTileValue>& source_tiles = index[source_url];
    std::map<uint64_t, RasterTileValue>::iterator it = source_tiles.find(tile_id);

    // The tile is already in this texture.
    if (it != source_tiles.end()) {
        RasterTileValue& value = it->second;
        return value.rect;
    }

    // The tile bitmap has zero width or height.
    if (!raster->width || !raster->height) {
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

    fprintf(stderr, "%llu: %i, %i, %i, %i\n", tile_id, rect.x, rect.y, rect.w, rect.h);

    if (rect.w == 0) {
        fprintf(stderr, "raster bitmap overflow");
        return rect;
    }

    assert(rect.x + rect.w <= width);
    assert(rect.y + rect.h <= height);

    source_tiles.emplace(tile_id, RasterTileValue { rect });

#if defined(DEBUG)
//    platform::show_debug_image("Raster Tile Atlas", data, width, height);
#endif

//    platform::show_debug_image("blah", raster.img->getData(), raster.img->getWidth(), raster.img->getHeight());

    // Copy the bitmap
    char *target = data;
    const char *source = raster->img->getData();
    for (uint32_t y = 0; y < buffered_height; y++) {
        uint32_t y1 = width * (rect.y + y) + rect.x;
        uint32_t y2 = buffered_width * y;
        for (uint32_t x = 0; x < buffered_width; x++) {
            target[y1 + x] = source[y2 + x];
//            fprintf(stderr, "wrote out %c (%i -> %i)\n", source[y2 + x], y2 + x, y1 + x);
        }
    }

//    const uint32_t *src_img = reinterpret_cast<const uint32_t *>(raster->img->getData());
//    uint32_t *dst_img = reinterpret_cast<uint32_t *>(data);
//
//    raster_copy_bitmap(
//        /* source buffer */  src_img,
//        /* source stride */  raster->img->getWidth(),
//        /* source x */       0,
//        /* source y */       0,
//        /* dest buffer */    dst_img,
//        /* dest stride */    raster->img->getWidth(),
//        /* dest x */         rect.x,
//        /* dest y */         rect.y,
//        /* icon dimension */ rect.w,
//        /* icon dimension */ rect.h
//    );

#if defined(DEBUG)
//    platform::show_debug_image("Raster Tile Atlas", data, width, height);
//    platform::show_debug_image(util::sprintf<12>("%llu", tile_id), reinterpret_cast<char *>(data), width, height);
#endif

    dirty = true;

//    fprintf(stderr, "add tile: %llu\n", tile_id);

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

//        fprintf(stderr, "remove tile: %llu\n", tile_id);
    }
}

void RasterTileAtlas::bind(Rect<uint16_t> rect) {
    if (rect.w && rect.h) {
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//            glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, rect.w, rect.h, GL_RGBA, GL_UNSIGNED_BYTE, data);
            dirty = false;
        }

#if defined(DEBUG)
//        platform::show_debug_image("Raster Tile Atlas", data, width, height);
#endif

    }
};
