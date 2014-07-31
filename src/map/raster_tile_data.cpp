#include <mbgl/map/map.hpp>
#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;


RasterTileData::RasterTileData(Tile::ID id, Map &map, const SourceInfo &source)
    : TileData(id, map, source),
    bucket() {
}

RasterTileData::~RasterTileData() {
    map.getRasterTileAtlas()->removeTile("test", id.to_uint64());
//    fprintf(stderr, "remove %llu\n", id.to_uint64());
}

void RasterTileData::parse() {
    if (state != State::loaded) {
        return;
    }

    if (bucket.setImage(data)) {
        map.getRasterTileAtlas()->addTile("test", id.to_uint64(), bucket.getImage());
//        fprintf(stderr, "add %llu\n", id.to_uint64());
        state = State::parsed;
    } else {
        state = State::invalid;
    }
}

void RasterTileData::render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc) {
//    Rect<uint16_t> rect = map.getRasterTileAtlas()->addTile("test", id.to_uint64(), bucket.getImage());
//    map.getRasterTileAtlas()->bind(rect);
    bucket.render(painter, layer_desc, id);
}

bool RasterTileData::hasData(std::shared_ptr<StyleLayer> layer_desc) const {
    return bucket.hasData();
}
