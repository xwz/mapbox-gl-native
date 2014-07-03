#include <llmr/map/map.hpp>
#include <llmr/map/raster_tile_data.hpp>
#include <llmr/style/layer_description.hpp>

using namespace llmr;

RasterTileData::RasterTileData(Tile::ID id, Map &map, const std::string url)
    : TileData(id, map, url),
    bucket(),
    id(id) {
}

RasterTileData::~RasterTileData() {
    map.getRasterTileAtlas()->removeTile("test", id.to_uint64());
}

void RasterTileData::parse() {
    if (state != State::loaded) {
        return;
    }

    if (bucket.setImage(data)) {
        map.getRasterTileAtlas()->addTile("test", id.to_uint64(), bucket.getImage());
        state = State::parsed;
    } else {
        state = State::invalid;
    }
}

void RasterTileData::render(Painter &painter, const LayerDescription& layer_desc) {
    Rect<uint16_t> rect = map.getRasterTileAtlas()->addTile("test", id.to_uint64(), bucket.getImage());
    map.getRasterTileAtlas()->bind(rect);
    bucket.render(painter, layer_desc.name, id);
}

bool RasterTileData::hasData(const LayerDescription& layer_desc) const {
    return bucket.hasData();
}
